#include <bits/stdc++.h>
#include "Interpreter.h"
#include "Parser.h"

using namespace std;
using namespace brainfuck;

#ifndef DETAILED
#include "CodeGen.h"
#include "ObjGen.h"
#include "llvm/Support/Path.h"

using llvm::sys::path::filename;
#endif

void printhelp(const string &name) {
  cerr << "OVERVIEW: A brainfuck compiler." << endl;
  cerr << "USAGE: " << name << " <input>" << endl;
  cerr << "Version 1.0" << endl;
  cerr << R"(
Copyright (C) 2019 Kirie Oikawa.
License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

Written by Kirie Oikawa.)"
       << endl;
}

int main(int argc, char *argv[]) {
  if (2 != argc) {
    printhelp(argv[0]);
    return 1;
  }

  string input(argv[1]);

  if ("--version" == input || "-v" == input || "-h" == input) {
    printhelp(argv[0]);
    return 1;
  }

  ifstream fin(input);
  if (!fin.good()) {
    perror(input.c_str());
    return 1;
  }

  vector<Instruction> inst;

  Parser parser(fin);
  parser.parse(inst);
  parser.compress(inst);

#ifndef DETAILED

  CodeGenLLVMIR cg;
  auto module = cg.codegen(input, inst);

  ObjectGenLLVMIR og;
  og.objgen(filename(input).str() + ".o", module.get());

#else

  Interpreter itp(cin, cout, cerr, DETAILED);
  itp.execute(inst);

#endif

  return 0;
}
