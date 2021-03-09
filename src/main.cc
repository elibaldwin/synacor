#include "synacor_vm.h"

#include <iostream>
#include <string>

int main() {
  SynacorVM vm = SynacorVM();
  vm.load("challenge.bin");
  //std::cout << vm.execute("");

  string in_line;
  while (getline(std::cin, in_line)) {
    if (in_line == "undo") {
      std::cout << vm.undo();
    } else {
      std::cout << vm.execute(in_line);
    }
  }
}