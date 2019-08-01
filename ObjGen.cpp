#include "ObjGen.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"

using namespace brainfuck;
using namespace llvm;
using namespace std;

ObjectGenLLVMIR::ObjectGenLLVMIR() {
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();
}

void ObjectGenLLVMIR::objgen(const string &name, Module *module) {
  // set target info
  auto TargetTriple = sys::getDefaultTargetTriple();
  module->setTargetTriple(TargetTriple);

  std::string Error;
  auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);
  if (!Target) {
    errs() << Error;
    abort();
  }
  auto CPU = "generic";
  auto Features = "";

  TargetOptions opt;
  auto RM = Optional<Reloc::Model>();
  auto TheTargetMachine =
      Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

  module->setDataLayout(TheTargetMachine->createDataLayout());

  // emit the object
  std::error_code EC;
  raw_fd_ostream dest(name, EC, sys::fs::F_None);

  if (EC) {
    errs() << "Could not open file: " << EC.message();
    abort();
  }

  legacy::PassManager pass;
  auto FileType = TargetMachine::CGFT_ObjectFile;

  if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
    errs() << "TheTargetMachine can't emit a file of this type";
    abort();
  }

  pass.run(*module);
  dest.flush();
}
