#include "synacor_vm.h"

SynacorVM::SynacorVM() : ip(0), sp(0) {
  reg.fill(0);
  stack.fill(0);
}

void SynacorVM::load(string bin_path) {
  std::ifstream infile(bin_path);
  infile.read((char*) mem.data(), mem.size() * sizeof(uint16_t));

  inp_stream = istringstream();
  out_stream = ostringstream();

  // step until vm is waiting for new input (or halts)
  while(step() && mem[ip]);

  str_state = out_stream.str();
}

string SynacorVM::execute(string command) {
  // make sure command ends with a newline character
  if (command.size() > 0 && command[command.size()-1] != '\n')
    command += '\n';
  
  if (!(command == "inv\n" || command == "look\n")) {
    snapshot state;
    state.command = command;
    state.output = str_state;
    state.ip = ip;
    state.sp = sp;
    state.reg = reg;
    state.stack = stack;
    state.mem = mem;

    history.push_back(state);
  }

  inp_stream = istringstream(command);
  out_stream = ostringstream();

  // step until vm is waiting for new input (or halts)
  while(step() && mem[ip]);

  if (command == "use teleporter\n") {
    used_teleporter = true;
  }

  if (command == "inv\n" || command == "look\n") {
    return out_stream.str(); // don't save output for actions that don't modify state
  }

  str_state = out_stream.str();

  return str_state;
}

string SynacorVM::undo() {
  snapshot state = history.back();

  ip = state.ip;
  sp = state.sp;
  reg = state.reg;
  stack = state.stack;
  mem = state.mem;

  str_state = state.output;

  history.pop_back();

  return str_state;
}

string SynacorVM::revert(int step) {
  if (step >= history.size())
    return str_state;
  
  snapshot state = history[step];

  ip = state.ip;
  sp = state.sp;
  reg = state.reg;
  stack = state.stack;
  mem = state.mem;

  str_state = state.output;

  history.resize(step);

  return str_state;
}

string SynacorVM::getState() {
  return str_state;
}

string SynacorVM::reset() {
  if (history.size() == 0) {
    return str_state;
  }

  snapshot state = history[0];

  ip = state.ip;
  sp = state.sp;
  reg = state.reg;
  stack = state.stack;
  mem = state.mem;

  str_state = state.output;

  history = vector<snapshot>();

  return str_state;
}

uint16_t SynacorVM::get(uint16_t addr) {
  uint16_t val = mem[addr];
  // check to see if val is a literal or refers to a register
  if (val & 0x8000) {
    // val refers to a register (first bit is set)
    if ((val & 0x7FFF) > 7) {
      std::cout << "invalid value " << val << " at address " << addr << "." << std::endl;
      exit(1);
    }
    return reg[val & 0x7FFF];
  } else {
    // val refers to a literal value; return it
    return val;
  }
}

bool SynacorVM::step() {
  if (used_teleporter) {
    if (ip == 5451) {
      reg[7] = 25734;
    }

    if (ip == 5489) {
      reg[0] = 6;
      ip+=2;
    }
  }

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
