#ifndef BRAINFUCK__CODEGEN__H
#define BRAINFUCK__CODEGEN__H

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"

#include "Instruction.h"

namespace brainfuck {

class CodeGen {
 public:
  virtual std::unique_ptr<llvm::Module> codegen(
      const std::string &name, const std::vector<Instruction> &inst) = 0;
  virtual ~CodeGen() = default;
};

class CodeGenLLVMIR : public CodeGen {
 public:
  virtual std::unique_ptr<llvm::Module> codegen(
      const std::string &name, const std::vector<Instruction> &inst) override;

 private:
  std::unique_ptr<llvm::LLVMContext> Context;
  std::unique_ptr<llvm::IRBuilder<>> Builder;
  std::unique_ptr<llvm::Module> Module;
  std::unique_ptr<llvm::DIBuilder> DBuilder;
  llvm::DICompileUnit *CU = nullptr;
  llvm::AllocaInst *dataPtr = nullptr;
  llvm::AllocaInst *dpPtr = nullptr;

  void initLLVM(const std::string &name);

  void generateLibfuncDecl();
  void generateMain(const std::vector<Instruction> &inst);
  void generateInst(std::vector<Instruction>::const_iterator begin,
                    std::vector<Instruction>::const_iterator end);
  llvm::Value *generateLoad();
  void generateStore(llvm::Value *val);
};

}  // namespace brainfuck

#endif
