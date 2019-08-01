#ifndef BRAINFUCK__INTERPRETER__H
#define BRAINFUCK__INTERPRETER__H

#include <iostream>
#include <vector>
#include "Instruction.h"

namespace brainfuck {

class Interpreter {
 public:
  Interpreter(std::istream &istrm, std::ostream &ostrm, std::ostream &estrm,
              bool detailed = false)
      : istrm(istrm), ostrm(ostrm), estrm(estrm), detailed(detailed) {}

  /// Execute the program.
  int execute(const std::vector<Instruction> &inst);

  /// Execute the instruction.
  int stepExec(const Instruction &i, size_t &ip);

  /// Clear the data storage.
  void clear();

  /// Dump current program status.
  void dump();

 private:
  std::vector<unsigned char> data{0};  // Store all data.
  size_t dp = 0;                       // Data pointer.
  std::istream &istrm;                 // Input stream.
  std::ostream &ostrm;                 // Output stream.
  std::ostream &estrm;                 // Error stream.
  bool detailed = false;               // Flag for detailed status dump.

  // Check whether the dp pointer is valid.
  bool goodPtr() const { return 0 <= dp && dp < data.size(); }
};

}  // namespace brainfuck

#endif
