#include "synacor_vm.h"

using std::ifstream;
using std::cout;
using std::cin;
using std::endl;

SynacorVM::SynacorVM(string bin_path) : ip(0), sp(0) {
  ifstream infile(bin_path);
  infile.read((char*) mem.data(), 65536);

  reg.fill(0);
  stack.fill(0);

  cout << "initialized vm with " << infile.gcount() << " bytes read from " << bin_path << " into memory." << endl;
}

string SynacorVM::execute(string command) {
  // make sure command ends with a newline character
  if (command.size() > 0 && command[command.size()-1] != '\n')
    command += '\n';
  
  snapshot state;
  state.command = command;
  state.output = out_stream.str();
  state.ip = ip;
  state.sp = sp;
  state.reg = reg;
  state.stack = stack;
  state.mem = mem;

  history.push_back(state);

  inp_stream = istringstream(command);
  out_stream = ostringstream();

  // step until vm is waiting for new input (or halts)
  while(step() && mem[ip]);

  return out_stream.str();
}

string SynacorVM::undo() {
  snapshot state = history.back();

  ip = state.ip;
  sp = state.sp;
  reg = state.reg;
  stack = state.stack;
  mem = state.mem;

  history.pop_back();

  return state.output;
}

uint16_t SynacorVM::get(uint16_t addr) {
  uint16_t val = mem[addr];
  // check to see if val is a literal or refers to a register
  if (val & 0x8000) {
    // val refers to a register (first bit is set)
    if ((val & 0x7FFF) > 7) {
      cout << "invalid value " << val << " at address " << addr << "." << endl;
      cout << (val & 0x7FFF);
      exit(1);
    }
    return reg[val & 0x7FFF];
  } else {
    // val refers to a literal value; return it
    return val;
  }
}

bool SynacorVM::step() {
  uint16_t op = mem[ip];

  switch (op) {
    case 1: // set
      reg[mem[ip+1] & 0x7FFF] = get(ip+2);
      ip+=3;
      break;

    case 2: // push
      stack[sp++] = get(ip+1);
      ip+=2;
      break;

    case 3: // pop
      reg[mem[ip+1] & 0x7FFF] = stack[--sp];
      ip+=2;
      break;

    case 4: // eq
      reg[mem[ip+1] & 0x7FFF] = (get(ip+2) == get(ip+3));
      ip+=4;
      break;

    case 5: // gt
      reg[mem[ip+1] & 0x7FFF] = (get(ip+2) > get(ip+3));
      ip+=4;
      break;

    case 6: // jmp
      ip = get(ip+1);
      break;

    case 7: // jt (jump true)
      if (get(ip+1)) {
        ip = get(ip+2);
      } else {
        ip+=3;
      }
      break;

    case 8: // jf (jump false)
      if (!get(ip+1)) {
        ip = get(ip+2);
      } else {
        ip+=3;
      }
      break;

    case 9: // add
      reg[mem[ip+1] & 0x7FFF] = (get(ip+2) + get(ip+3)) & 0x7FFF;
      ip+=4;
      break;

    case 10:// mult
      reg[mem[ip+1] & 0x7FFF] = (get(ip+2) * get(ip+3)) & 0x7FFF;
      ip+=4;
      break;

    case 11:// mod
      reg[mem[ip+1] & 0x7FFF] = get(ip+2) % get(ip+3);
      ip+=4;
      break;

    case 12:// and
      reg[mem[ip+1] & 0x7FFF] = get(ip+2) & get(ip+3);
      ip+=4;
      break;

    case 13:// or
      reg[mem[ip+1] & 0x7FFF] = get(ip+2) | get(ip+3);
      ip+=4;
      break;
      
    case 14:// not
      reg[mem[ip+1] & 0x7FFF] = get(ip+2) ^ 0x7FFF;
      ip+=3;
      break;

    case 15:// rmem
      reg[mem[ip+1] & 0x7FFF] = mem[get(ip+2)];
      ip+=3;
      break;

    case 16:// wmem
      mem[get(ip+1)] = get(ip+2);
      ip+=3;
      break;

    case 17:// call
      stack[sp++] = ip+2;
      ip = get(ip+1);
      break;

    case 18:// ret
      ip = stack[--sp];
      break;

    case 19:// out
      {
        char c = (char) get(ip+1);
        out_stream << c;
        ip += 2;
        break;
      } 

    case 20:// in
      {
        char c;
        if (!inp_stream.get(c)) {
          return false; // ran out of input; return without incrementing
        };
        reg[mem[ip+1] & 0x7FFF] = c;
        ip+=2;
        break;
      }

    case 21:// nop
      ip++;
      break;
  }

  return true;
}

int main() {
  string in_line;

  SynacorVM vm("challenge.bin");
  cout << vm.execute("");

  while (getline(cin, in_line)) {
    if (in_line == "undo") {
      cout << vm.undo();
    } else {
      cout << vm.execute(in_line);
    }
  }
}