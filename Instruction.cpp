#include "Instruction.h"

using namespace std;
using namespace brainfuck;

Instruction::Kind Instruction::getKind(const char inst) {
  switch (inst) {
#define INSTRUCTION_KIND(Name, Char) \
  case Char:                         \
    return Name;
#include "Instruction.def"
  }
  return nop;
}

ostream &brainfuck::operator<<(ostream &out, const Instruction &o) {
  const char *instStr[] = {
#define INSTRUCTION_KIND(Name, Char) #Name,
#include "Instruction.def"
  };
#define INSTRUCTION_CONSTANT(Name, Val) \
  const int inst##Name = int(Instruction::Val);
#include "Instruction.def"
  int inst = o.getInst();
  if (instbegin <= inst && inst <= instend) {
    out << instStr[o.getInst()];
    int argv = o.getArgv();
    if (argv) {
      out << ", " << argv;
    }
  } else {
    out << "<unknown instruction>";
  }
  return out;
}
