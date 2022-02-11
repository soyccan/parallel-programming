#include "utils.h"

int ret[128];
int run(int n, int key) {
  int sum = 0;

  // x86 has 64-byte cache line
  // separate the 4 result arrays to different cache lines
  // otherwise 4 threads will race the same cache line
  // and produce terrible performance
  f(n, key, ret, ret + 16, ret + 32, ret + 48);

  for (int i = 0; i < 64; i += 16)
    sum += ret[i];

  return sum;
}
