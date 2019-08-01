#include "Parser.h"
#include <cassert>
#include <stack>

using namespace std;
using namespace brainfuck;

void Parser::parse(vector<Instruction> &instructions) {
  stack<int> nestedIndex;  // Record the indexes of nested open braces.

  char chinst = 0;
  while (ins.get(chinst)) {
    instructions.emplace_back(chinst);

    switch (instructions.back().getInst()) {
      case Instruction::nop:  // For unrecognized chars, remove the inst.
        instructions.pop_back();
        break;

      case Instruction::fjp:  // Record current index.
        nestedIndex.push(instructions.size() - 1);
        break;

      case Instruction::bjp: {
        // Compute the shift, and pop the nested index.
        int shift = nestedIndex.top() - instructions.size() + 1;
        instructions.back().setArgv(shift);
        instructions[nestedIndex.top()].setArgv(-shift);
        nestedIndex.pop();
      } break;

      default:
        /* nothing to do */
        break;
    }
  }
}

void Parser::compress(vector<Instruction> &instructions) {
  stack<int> nestedIndex;  // Record the indexes of nested open braces.

  vector<Instruction> output;
  Instruction current;

  auto addinst = [&](Instruction::Kind kind = Instruction::nop) {
    if (current.getInst()) {
      output.push_back(current);
    }
    current = Instruction(kind);
  };

  for (auto &i : instructions) {
    switch (i.getInst()) {
      case Instruction::fjp:
        addinst();
        output.emplace_back(i);  // However, the argv is incorrect.
        nestedIndex.push(output.size() - 1);
        break;

      case Instruction::bjp: {
        addinst();
        int matched = nestedIndex.top();
        assert(output[matched].getArgv() == -i.getArgv());
        int shift = matched - output.size();
        output.emplace_back(Instruction::bjp, shift);
        output[matched].setArgv(-shift);
        nestedIndex.pop();
      } break;

      case Instruction::ipt:
      case Instruction::opt:
        addinst();
        output.emplace_back(i);  // However, the argv is incorrect.
        break;

      default:
        if (i.getInst() != current.getInst()) {
          addinst(i.getInst());
        }
        current.setArgv(current.getArgv() + 1);
        break;
    }
  }

  instructions = output;
}
