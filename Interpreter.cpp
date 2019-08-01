#include "Interpreter.h"
#include <cassert>

using namespace std;
using namespace brainfuck;

void Interpreter::clear() {
  data.clear();
  data.emplace_back(0);
  dp = 0;
}

int Interpreter::execute(const vector<Instruction> &inst) {
  clear();

  size_t isize = inst.size();
  for (size_t ip = 0; ip < isize; ++ip) {
    int ret = stepExec(inst[ip], ip);
    if (ret) return ret;
  }

  return 0;
}

int Interpreter::stepExec(const Instruction &i, size_t &ip) {
  // If executed in detail, dump program status.
  if (detailed) {
    dump();
    estrm << "  next instruction: " << i << endl;
  }

  // Check whether the dp pointer is still valid, abort if not.
  if (!goodPtr()) {
    estrm << "abort: data pointer out of bound: " << dp << endl;
    return 1;
  }

  char iochar = 0;  // used to collect the input/output char
  int argv = i.getArgv();
  switch (i.getInst()) {
    case Instruction::nop:
      /* nothing to do */
      break;

    case Instruction::add:
      data[dp] += argv ? argv : 1;
      break;

    case Instruction::sub:
      data[dp] -= argv ? argv : 1;
      break;

    case Instruction::lsh:
      // We do not allow underflow, negative index will abort in the next loop.
      dp -= argv ? argv : 1;
      break;

    case Instruction::rsh:
      dp += argv ? argv : 1;
      // If overflew, add new bytes.
      if (!goodPtr()) data.resize(dp + 1);
      break;

    case Instruction::fjp:
      // Skip the loop body if false.
      ip += data[dp] ? 0 : argv;
      break;

    case Instruction::bjp:
      // Exit the loop and continue if false.
      ip += data[dp] ? argv : 0;
      break;

    case Instruction::ipt:
      // Input a char.
      assert(0 == argv);
      istrm.get(iochar);
      data[dp] = iochar;
      break;

    case Instruction::opt:
      // Output current char.
      assert(0 == argv);
      iochar = data[dp];
      if (detailed) estrm << "> ";
      ostrm << iochar;
      if (detailed) estrm << endl;
      break;
  }
  return 0;
}

void Interpreter::dump() {
  estrm << "Current status:" << endl;

  // dump data
  estrm << "  data = {" << endl;
  int i = 0;
  for (unsigned char d : data) {
    estrm << "    [" << i++ << "] = " << (unsigned int)d << endl;
  }
  estrm << "  }" << endl;

  // dump dp
  estrm << "  dp = " << dp << endl;
}
