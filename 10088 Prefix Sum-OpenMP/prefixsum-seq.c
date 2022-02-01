#include "utils.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXN 10000005
#define MAX_THREAD 4

uint32_t prefix_sum[MAXN];

int main() {
  int n;
  uint32_t key;
  while (scanf("%d %" PRIu32, &n, &key) == 2) {
    uint32_t sum = 0;
    for (int i = 1; i <= n; i++) {
      sum += encrypt(i, key);
      prefix_sum[i] = sum;
    }
    output(prefix_sum, n);
  }
  return 0;
}
