#ifndef BRAINFUCK__PARSER__H
#define BRAINFUCK__PARSER__H

#include <iostream>
#include <vector>
#include "Instruction.h"

namespace brainfuck {

class Parser {
 public:
  Parser(std::istream &ins) : ins(ins) {}

  void parse(std::vector<Instruction> &instructions);

  void compress(std::vector<Instruction> &instructions);

 private:
  std::istream &ins;
};

}  // namespace brainfuck

#endif
