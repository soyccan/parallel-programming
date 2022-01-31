#define _GNU_SOURCE

#ifdef __linux__
#include <sched.h>
#endif

#include "utils.h"

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#define NDEBUG

#ifndef NDEBUG
#define log(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#else
#define log(...)
#endif

#ifndef min
#define min(x, y)                                                              \
  ({                                                                           \
    __auto_type _min1 = (x);                                                   \
    __auto_type _min2 = (y);                                                   \
    (void)(&_min1 == &_min2);                                                  \
    _min1 < _min2 ? _min1 : _min2;                                             \
  })
#endif

#ifndef ceildivi
#define ceildivi(x, y)                                                         \
  ({                                                                           \
    __auto_type _x = (x);                                                      \
    __auto_type _y = (y);                                                      \
    (_x + _y - 1) / _y;                                                        \
  })
#endif

#define MAXN 10000005
#define MAX_THREAD 6

struct argument {
  uint32_t *prefix;
  int start_idx;
  int len;
  union {
    uint32_t val;
    uint32_t key;
  };
};

void *calc_prefix_sub(void *arg_) {
  struct argument *arg = arg_;
  uint32_t sum = 0;
  log("calc_prefix_sub start_idx=%d len=%d key=%u", arg->start_idx, arg->len,
      arg->key);
  for (int i = 0; i < arg->len; i++) {
    sum += encrypt(arg->start_idx + i, arg->key);
    arg->prefix[i] = sum;
  }
  return NULL;
}

void *merge_prefix(void *arg_) {
  struct argument *arg = arg_;
  log("merge_prefix start_idx=%d len=%d val=%u", arg->start_idx, arg->len,
      arg->val);
  for (int i = 0; i < arg->len; i++) {
    arg->prefix[i] += arg->val;
  }
  return NULL;
}

uint32_t prefix_sum[MAXN];

// put arguments at global is faster
struct argument arguments[MAX_THREAD];

int main() {
#ifdef __linux__
  // only dispatch to cores on the same CPU for stability
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  for (int i = 0; i < 6; i++)
    CPU_SET(i, &cpuset);
  assert(sched_setaffinity(0, sizeof(cpuset), &cpuset) == 0);
#endif

  int n;
  uint32_t key;

  pthread_t pool[MAX_THREAD];

  log("MAX_THREAD=%d", MAX_THREAD);

  while (scanf("%d %" PRIu32, &n, &key) == 2) {
    int block_size = ceildivi(n, MAX_THREAD);
    int num_thread = ceildivi(n, block_size);
    int remain_size = num_thread * block_size - n;

    log("n=%d block_size=%d num_thread=%d remain_size=%d", n, block_size,
        num_thread, remain_size);

    // calculate prefix sum of each subinterval
    for (int i = 0; i < num_thread; i++) {
      int start_idx = 1 + block_size * i;
      arguments[i].prefix = prefix_sum + start_idx;
      arguments[i].start_idx = start_idx;
      arguments[i].len = block_size - (i == num_thread - 1) * remain_size;
      arguments[i].key = key;
      pthread_create(&pool[i], NULL, calc_prefix_sub, &arguments[i]);
    }
    for (int i = 0; i < num_thread; i++) {
      pthread_join(pool[i], NULL);
    }

    // merge subinterval prefix in tree structure
    for (int step = 1; step < MAX_THREAD; step *= 2) {
      int num_thread_2 = num_thread - step;
      for (int i = 0; i < num_thread_2; i++) {
        // get all val before launching threads to prevent race condition
        int val_idx = 1 + block_size * (i + 1) - 1;
        arguments[i + step].val = prefix_sum[val_idx];
      }
      for (int i = 0; i < num_thread_2; i++) {
        pthread_create(&pool[i], NULL, merge_prefix, &arguments[i + step]);
      }
      for (int i = 0; i < num_thread_2; i++) {
        pthread_join(pool[i], NULL);
      }
      log("");
    }

    output(prefix_sum, n);
  }
  return 0;
}
