#include "CodeGen.h"
#include "llvm/IR/Verifier.h"

using namespace brainfuck;
using namespace llvm;
using namespace std;

#define type(kind) Type::get##kind##Ty(*Context)

void CodeGenLLVMIR::initLLVM(const string &name) {
  Context = llvm::make_unique<LLVMContext>();
  Builder = llvm::make_unique<IRBuilder<>>(*Context);
  Module = llvm::make_unique<llvm::Module>(name, *Context);
}

unique_ptr<Module> CodeGenLLVMIR::codegen(const string &name,
                                          const vector<Instruction> &inst) {
  initLLVM(name);
  generateLibfuncDecl();
  generateMain(inst);
  return move(Module);
}

void CodeGenLLVMIR::generateLibfuncDecl() {
  // Declare getchar: int getchar()
  Function::Create(FunctionType::get(type(Int32), {}, /* isVarArg = */ false),
                   Function::ExternalLinkage, "getchar", Module.get());

  // Declare putchar: int putchar(int)
  Function::Create(
      FunctionType::get(type(Int32), {type(Int32)}, /* isVarArg = */ false),
      Function::ExternalLinkage, "putchar", Module.get());

  // Declare dplsh: void dplsh(size_t *, size_t)
  Function::Create(FunctionType::get(type(Void), {type(Int64Ptr), type(Int64)},
                                     /* isVarArg = */ false),
                   Function::ExternalLinkage, "dplsh", Module.get());

  // Declare dprsh: char *dplsh(size_t *, size_t)
  Function::Create(
      FunctionType::get(type(Int8Ptr), {type(Int64Ptr), type(Int64)},
                        /* isVarArg = */ false),
      Function::ExternalLinkage, "dprsh", Module.get());

  // Declare main: int main()
  Function::Create(FunctionType::get(type(Int32), {}, /* isVarArg = */ false),
                   Function::ExternalLinkage, "main", Module.get());
}

void CodeGenLLVMIR::generateMain(const vector<Instruction> &inst) {
  Function *fpmain = Module->getFunction("main");
  assert(fpmain);
  assert(fpmain->empty());

  BasicBlock *entry = BasicBlock::Create(*Context, "entry", fpmain);
  Builder->SetInsertPoint(entry);

  /* beginning of generating instructions */

  // Generate data and dp, store pointer of data[dp] to shift.
  Value *temp = nullptr;
  // allocate data and dp
  dataPtr = Builder->CreateAlloca(type(Int8Ptr), nullptr, "dataPtr");
  dpPtr = Builder->CreateAlloca(type(Int64), nullptr, "dpPtr");
  // dp = -1, data = dprsh(&dp, 1)
  Builder->CreateStore(ConstantInt::get(type(Int64), -1), dpPtr);
  temp = Builder->CreateCall(Module->getFunction("dprsh"),
                             {dpPtr, ConstantInt::get(type(Int64), 1)}, "init");
  Builder->CreateStore(temp, dataPtr);

  // Create a new BB for better view.
  entry = BasicBlock::Create(*Context, "start", fpmain);
  Builder->CreateBr(entry);
  Builder->SetInsertPoint(entry);
  generateInst(inst.begin(), inst.end());

  /* ending of generating instructions */

  // create `return 0;` for main.
  Builder->CreateRet(ConstantInt::get(type(Int32), 0));

  verifyFunction(*fpmain);
}

void CodeGenLLVMIR::generateInst(vector<Instruction>::const_iterator begin,
                                 vector<Instruction>::const_iterator end) {
  Value *temp = nullptr;

  for (auto i = begin; end != i; ++i) {
    int argv = i->getArgv();

    switch (i->getInst()) {
      default:
        assert(!"bad instruction kind");
        break;

      case Instruction::add:
        temp = generateLoad();
        temp = Builder->CreateAdd(temp, ConstantInt::get(type(Int8), argv));
        generateStore(temp);
        break;

      case Instruction::sub:
        temp = generateLoad();
        temp = Builder->CreateSub(temp, ConstantInt::get(type(Int8), argv));
        generateStore(temp);
        break;

      case Instruction::lsh:
        Builder->CreateCall(Module->getFunction("dplsh"),
                            {dpPtr, ConstantInt::get(type(Int64), argv)});
        break;

      case Instruction::rsh:
        temp =
            Builder->CreateCall(Module->getFunction("dprsh"),
                                {dpPtr, ConstantInt::get(type(Int64), argv)});
        Builder->CreateStore(temp, dataPtr);
        break;

      case Instruction::fjp: {
        auto *func = Module->getFunction("main");
        // generate BB for condition
        BasicBlock *cond = BasicBlock::Create(*Context, "cond", func);
        Builder->CreateBr(cond);
        Builder->SetInsertPoint(cond);
        // add condition for fjp in condition BB
        temp = generateLoad();
        temp = Builder->CreateICmpNE(temp, ConstantInt::get(type(Int8), 0));
        BasicBlock *loop = BasicBlock::Create(*Context, "loop", func);
        BasicBlock *after = BasicBlock::Create(*Context, "after");
        Builder->CreateCondBr(temp, loop, after);
        // shift insert point to loop body
        // and recursive invoke generateInst to generate loop body
        Builder->SetInsertPoint(loop);
        generateInst(i + 1, i + argv);
        i += argv;  // shift pointer to skip the loop body.
        // add condition for bjp in loop BB
        Builder->SetInsertPoint(loop);
        Builder->CreateBr(cond);
        // add after BB, and shift insert point
        func->getBasicBlockList().push_back(after);
        Builder->SetInsertPoint(after);
      } break;

      case Instruction::bjp:
        /* nothing to do */
        break;

      case Instruction::ipt:
        temp = Builder->CreateCall(Module->getFunction("getchar"), {});
        temp = Builder->CreateTrunc(temp, type(Int8));
        generateStore(temp);
        break;

      case Instruction::opt:
        temp = generateLoad();
        temp = Builder->CreateSExt(temp, type(Int32));
        Builder->CreateCall(Module->getFunction("putchar"), {temp});
        break;
    }
  }
}

Value *CodeGenLLVMIR::generateLoad() {
  Value *data = Builder->CreateLoad(dataPtr, "data");
  Value *dp = Builder->CreateLoad(dpPtr, "dp");
  Value *shift = Builder->CreateGEP(data, dp, "shift");
  return Builder->CreateLoad(shift);
}

void CodeGenLLVMIR::generateStore(Value *val) {
  Value *data = Builder->CreateLoad(dataPtr, "data");
  Value *dp = Builder->CreateLoad(dpPtr, "dp");
  Value *shift = Builder->CreateGEP(data, dp, "shift");
  Builder->CreateStore(val, shift);
}
