#ifndef BRAINFUCK__INSTRUCTION__H
#define BRAINFUCK__INSTRUCTION__H

#include <iostream>

namespace brainfuck {

class Instruction {
 public:
  enum Kind {
#define INSTRUCTION_KIND(Name, Char) Name,
#include "Instruction.def"
  };
  /// Convert the char style instruction to instruction enumerate.
  static Kind getKind(const char inst);

  Instruction() = default;
  Instruction(const char inst) : inst(getKind(inst)) {}
  Instruction(const Kind inst) : inst(inst) {}
  Instruction(const Kind inst, int argv) : inst(inst), argv(argv) {}

  bool isNop() const { return nop == inst; }
  bool isJmp() const { return fjp == inst || bjp == inst; }
  Kind getInst() const { return inst; }
  int getArgv() const { return argv; }
  void setArgv(const int argv) { this->argv = argv; }

  bool operator==(const Instruction &o) {
    return inst == o.inst && argv == o.argv;
  }

 private:
  Kind inst = nop;
  int argv = 0;

  friend std::ostream &operator<<(std::ostream &out, const Instruction &o);
};

std::ostream &operator<<(std::ostream &out, const Instruction &o);

}  // namespace brainfuck

#endif
