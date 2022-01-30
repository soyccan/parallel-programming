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

#ifndef NDEBUG
#define log(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#else
#define log(...)
#endif

#define MAXN 10000005
#define MAX_THREAD 4

struct argument {
  uint32_t *prefix;
  int start_idx;
  int len;
  union {
    int val;
    int key;
  };
} arguments[MAX_THREAD];

void *calc_prefix_sub(void *arg_) {
  struct argument *arg = arg_;
  uint32_t sum = 0;
  log("calc_prefix_sub start_idx=%d len=%d key=%d", arg->start_idx, arg->len,
      arg->key);
  for (int i = 0; i < arg->len; i++) {
    sum += encrypt(arg->start_idx + i, arg->key);
    arg->prefix[i] = sum;
  }
  return NULL;
}

void *merge_prefix(void *arg_) {
  struct argument *arg = arg_;
  log("merge_prefix start_idx=%d len=%d val=%d", arg->start_idx, arg->len,
      arg->val);
  for (int i = 0; i < arg->len; i++) {
    arg->prefix[i] += arg->val;
  }
  return NULL;
}

int n;
uint32_t key;

uint32_t prefix_sum[MAXN];
pthread_t pool[MAX_THREAD];

int main() {
#ifdef __linux__
  // only dispatch to CPU 0-5 for stability
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  for (int i = 0; i < 6; i++)
    CPU_SET(i, &cpuset);
  assert(sched_setaffinity(0, sizeof(cpuset), &cpuset) == 0);
#endif

  while (scanf("%d %" PRIu32, &n, &key) == 2) {
    int block_size = (n + MAX_THREAD - 1) / MAX_THREAD;
    int remain_size = MAX_THREAD * block_size - n;

    // calculate prefix sum of each subinterval
    for (int i = 0; i < MAX_THREAD; i++) {
      int start_idx = 1 + block_size * i;
      arguments[i].prefix = prefix_sum + start_idx;
      arguments[i].start_idx = start_idx;
      arguments[i].len = block_size - (i == MAX_THREAD - 1) * remain_size;
      arguments[i].key = key;
      pthread_create(&pool[i], NULL, calc_prefix_sub, &arguments[i]);
    }
    for (int i = 0; i < MAX_THREAD; i++) {
      pthread_join(pool[i], NULL);
    }

    // merge subinterval prefix in tree structure
    for (int step = 1; step < MAX_THREAD; step *= 2) {
      int num_thread = MAX_THREAD - step;
      for (int i = 0; i < num_thread; i++) {
        int start_idx = 1 + block_size * (i + step);
        int val_idx = 1 + block_size * (i + 1) - 1;
        arguments[i].prefix = prefix_sum + start_idx;
        arguments[i].start_idx = start_idx;
        arguments[i].len = block_size - (i == num_thread - 1) * remain_size;
        arguments[i].val = prefix_sum[val_idx];
        pthread_create(&pool[i], NULL, merge_prefix, &arguments[i]);
      }
      for (int i = 0; i < num_thread; i++) {
        pthread_join(pool[i], NULL);
      }
    }

    output(prefix_sum, n);
  }
  return 0;
}
