#ifndef BRAINFUCK__OBJGEN__H
#define BRAINFUCK__OBJGEN__H

#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

namespace brainfuck {

class ObjectGenLLVMIR {
 public:
  ObjectGenLLVMIR();
  void objgen(const std::string &name, llvm::Module *module);
};

}  // namespace brainfuck

#endif
