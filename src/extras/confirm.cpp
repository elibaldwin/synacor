
#include <cstdint>
#include <iostream>

#define N 4
#define M 1
#define MOD 32768

uint16_t C[N+1][MOD];

int ack_mod(uint16_t c) {
  uint16_t temp;
  for (int j = 1; j < N+1; j++) {
    C[j][0] = C[j-1][c];
    for (int i = 1; i < MOD; i++) {
      temp = C[j][i-1] % MOD;
      C[j][i] = C[j-1][temp];
      if (i == M && j == N) {
        return C[N][M];
      }
    }
  }
}

int main() {
  for (int i = 0; i < MOD; i++) {
    C[0][i] = i+1;
  }

  for (int c = 1; c < MOD; c++) {
    if (c % 4096 == 4095) {
      std::cout << c << std::endl;
    }
    if (ack_mod(c) == 6) {
      std::cout << "found answer: " << c << std::endl;
      break;
    }
  }
}