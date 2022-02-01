#include "utils.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define MAXN 10000005
#define MAX_THREAD 4

#define NDEBUG

#ifndef NDEBUG
#define log(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#else
#define log(...)
#endif

#define ceildivi(x, y)                                                         \
  ({                                                                           \
    __auto_type _x = (x);                                                      \
    __auto_type _y = (y);                                                      \
    (_x + _y - 1) / _y;                                                        \
  })

uint32_t prefix_sum[MAXN];

// prefix sum of subinterval sum
// the 2nd dimension is to solve cache false sharing
// (cache line is 64-byte in x86)
uint32_t prefix_sum_sub[MAX_THREAD];
uint32_t prefix_sum_sub_tmp[MAX_THREAD][16];

int main() {
#ifdef _OPENMP
  omp_set_num_threads(MAX_THREAD);
#endif

  int n;
  uint32_t key;

  while (scanf("%d %" PRIu32, &n, &key) == 2) {
    uint32_t sum = 0;
#pragma omp parallel for schedule(static) firstprivate(sum)
    // correctness rely on a static (round-robin) scheduling policy
    for (int i = 1; i <= n; i++) {
      sum += encrypt(i, key);
      prefix_sum[i] = sum;

#ifdef _OPENMP
      int thread_id = omp_get_thread_num();
#else
      int thread_id = 0;
#endif

      prefix_sum_sub_tmp[thread_id][0] = sum;

      log("thread=%d i=%d sum=%d prefix_sum_sub[%d]=%d", thread_id, i, sum,
          thread_id, sum);
    }

    sum = 0;
    for (int i = 0; i < MAX_THREAD; i++) {
      sum += prefix_sum_sub_tmp[i][0];
      prefix_sum_sub[i] = sum;
    }

    //     for (int step = 1; step < MAX_THREAD; step *= 2) {
    // #pragma omp parallel for
    //       for (int i = 0; i < MAX_THREAD; i++) {
    //         prefix_sum_sub_tmp[i][0] = prefix_sum_sub[i][0];
    //       }
    //
    // #pragma omp parallel for
    //       for (int i = 0; i < MAX_THREAD - step; i++) {
    //         prefix_sum_sub[i + step][0] += prefix_sum_sub_tmp[i][0];
    //         log("prefix_sum_sub[%d]=%d", i+step, prefix_sum_sub[i+step][0]);
    //       }
    //       log("");
    //     }

#pragma omp parallel for schedule(static)
    for (int i = 1; i <= n; i++) {
#ifdef _OPENMP
      int thread_id = omp_get_thread_num();
#else
      int thread_id = 0;
#endif

      if (thread_id > 0)
        prefix_sum[i] += prefix_sum_sub[thread_id - 1];

      log("thread=%d i=%d prefix_sum[%d]=%d", thread_id, i, i, prefix_sum[i]);
    }

    output(prefix_sum, n);
  }
  return 0;
}
