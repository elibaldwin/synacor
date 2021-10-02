#include <fstream>
#include <iostream>
#include <cstdint>

using namespace std;

uint16_t MEM[65536];

int load_program(char* path) {
  ifstream infile(path);
  infile.read((char*) MEM, sizeof(MEM));

  int chars_read = infile.gcount();
  cout << "read " << chars_read << " bytes from " << path << " into memory." << endl;
}

uint16_t get(uint16_t addr, ofstream &ofs) {
  uint16_t val = MEM[addr];
  if (val & 0x8000) { // register value
    ofs << " <" << (val & 0x7FFF) << ">";
  } else { // literal
    ofs << " " << val;
  }
}

void disassemble_program() {
  ofstream ofs ("code_dump.txt", ofstream::out);

  uint16_t ip = 0;
  while (ip < 60100) {
    uint16_t op = MEM[ip];

    if (op > 21) {
      //cout << ip << ": unrecognized opcode: " << op << endl;
      ofs << ip << ": data value " << op << endl; 
      ip++;
    } else {
      ofs << ip << ": ";
      switch (op) {
        case 0:
          ofs << "halt" << endl;
          while (MEM[ip] == 0 && ip < 60100) {
            ip++;
          }
          break;
        case 1: // set
          ofs << "set";
          get(ip+1, ofs);
          get(ip+2, ofs);
          ip+=3;
          break;
        case 2: // push
          ofs << "push";
          get(ip+1, ofs);
          ip+=2;
          break;
        case 3: // pop
          ofs << "pop";
          get(ip+1, ofs);
          ip+=2;
          break;
        case 4: // eq
          ofs << "eq";
          get(ip+1, ofs);
          get(ip+2, ofs);
          get(ip+3, ofs);
          ip+=4;
          break;
        case 5: // gt
          ofs << "gt";
          get(ip+1, ofs);
          get(ip+2, ofs);
          get(ip+3, ofs);
          ip+=4;
          break;
        case 6: // jmp
          ofs << "jmp";
          get(ip+1, ofs);
          ip+=2;
          break;
        case 7: // jt (jump true)
          ofs << "jt";
          get(ip+1, ofs);
          get(ip+2, ofs);
          ip+=3;
          break;
        case 8: // jf (jump false)
          ofs << "jf";
          get(ip+1, ofs);
          get(ip+2, ofs);
          ip+=3;
          break;
        case 9: // add
          ofs << "add";
          get(ip+1, ofs);
          get(ip+2, ofs);
          get(ip+3, ofs);
          ip+=4;
          break;
        case 10:// mult
          ofs << "mult";
          get(ip+1, ofs);
          get(ip+2, ofs);
          get(ip+3, ofs);
          ip+=4;
          break;
        case 11:// mod
          ofs << "mod";
          get(ip+1, ofs);
          get(ip+2, ofs);
          get(ip+3, ofs);
          ip+=4;
          break;
        case 12:// and
          ofs << "and";
          get(ip+1, ofs);
          get(ip+2, ofs);
          get(ip+3, ofs);
          ip+=4;
          break;
        case 13:// or
          ofs << "or";
          get(ip+1, ofs);
          get(ip+2, ofs);
          get(ip+3, ofs);
          ip+=4;
          break;
        case 14:// not
          ofs << "not";
          get(ip+1, ofs);
          get(ip+2, ofs);
          ip+=3;
          break;
        case 15:// rmem
          ofs << "rmem";
          get(ip+1, ofs);
          get(ip+2, ofs);
          ip+=3;
          break;
        case 16:// wmem
          ofs << "wmem";
          get(ip+1, ofs);
          get(ip+2, ofs);
          ip+=3;
          break;
        case 17:// call
          ofs << "call";
          get(ip+1, ofs);
          ip+=2;
          break;
        case 18:// ret
          ofs << "ret";
          ip+=1;
          break;
        case 19:// out
          ofs << "out";
          get(ip+1, ofs);
          ip+=2;
          break;
        case 20:// in
          ofs << "in";
          get(ip+1, ofs);
          ip+=2;
          break;
        case 21:// nop
          ofs << "nop";
          ip++;
          break;
      }
      ofs << endl;
    }
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    cout << "Usage: ./vm <program file>" << endl;
  }

  load_program(argv[1]);

  disassemble_program();
}