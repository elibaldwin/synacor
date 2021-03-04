#ifndef SYNACOR_VM_H
#define SYNACOR_VM_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <string>
#include <array>
#include <vector>

using std::string;
using std::array;
using std::vector;

using std::istringstream;
using std::ostringstream;

struct snapshot {
  string command, output;
  uint16_t ip, sp;
  array<uint16_t, 8> reg;
  array<uint16_t, 256> stack;
  array<uint16_t, 32768> mem; 
};

class SynacorVM {

  // Memory has 15-bit address space; i.e., memory size = 2^15
  array<uint16_t, 32768> mem;
  // Stack is 'unbounded', but we can get away with a relatively small fixed size
  array<uint16_t, 256>   stack;
  // 8 registers
  array<uint16_t, 8>     reg;

  uint16_t ip;                // instruction pointer
  uint16_t sp;                // stack pointer

  istringstream inp_stream;   // text input  stream
  ostringstream out_stream;   // text output stream

  vector<snapshot> history;

  public:
    SynacorVM(string bin_path);      // construct VM using binary code file at bin_path
    string execute(string command);  // execute 'command' as input, return output
    string undo();                   // make one step back into state history
  
  private:
    bool step();
    uint16_t get(uint16_t addr);
};

#endif // SYNACOR_VM_H