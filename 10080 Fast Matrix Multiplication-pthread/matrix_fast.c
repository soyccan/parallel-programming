#include "matrix.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>

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

#define MAX_THREAD 12

struct argument {
  const unsigned long (*A)[2048];
  const unsigned long (*B)[2048];
  unsigned long (*C)[2048];
  int si, sj;
  int block_size;
  int N;
  int thread_id;
} arguments[MAX_THREAD];

pthread_t pool[MAX_THREAD];

void *multiply_block(void *arg_) {
  struct argument *arg = arg_;

  const unsigned long(*A)[2048] = arg->A;
  const unsigned long(*B)[2048] = arg->B;
  unsigned long(*C)[2048] = arg->C;
  int si = arg->si;
  int sj = arg->sj;
  int block_size = arg->block_size;
  int N = arg->N;
  int thread_id = arg->thread_id;

  for (int sk = 0; sk < N; sk += block_size) {
    log("multiply_block tid=%d si=%d sj=%d sk=%d block_size=%d N=%d", thread_id,
        si, sj, sk, block_size, N);
    for (int i = si; i < si + block_size && i < N; i++) {
      for (int j = sj; j < sj + block_size && j < N; j++) {
        unsigned long cij = C[i][j];
        for (int k = sk; k < sk + block_size && k < N; k++) {
          log("  tid=%1$d C[%2$d][%3$d] += A[%2$d][%4$d] * B[%4$d][%3$d]",
              thread_id, i, j, k);
          cij += A[i][k] * B[k][j];
        }
        C[i][j] = cij;
      }
    }
  }
  return NULL;
}

void multiply(int N, const unsigned long A[][2048],
              const unsigned long B[][2048], unsigned long C[][2048]) {
  int sqrt_max_thread = sqrt(MAX_THREAD);
  int block_size = ceildivi(N, sqrt_max_thread);
  int num_block = ceildivi(N, block_size);
  int remain_size = block_size * num_block - N;

  log("N=%d block_size=%d num_block=%d remain_size=%d", N, block_size,
      num_block, remain_size);

  memset(C, 0, sizeof(C[0]) * N);

  int num_thread = 0;
  for (int si = 0; si < N; si += block_size) {
    for (int sj = 0; sj < N; sj += block_size) {
      arguments[num_thread].A = A;
      arguments[num_thread].B = B;
      arguments[num_thread].C = C;
      arguments[num_thread].si = si;
      arguments[num_thread].sj = sj;
      arguments[num_thread].block_size = block_size;
      arguments[num_thread].N = N;
      arguments[num_thread].thread_id = num_thread;
      pthread_create(&pool[num_thread], NULL, multiply_block,
                     &arguments[num_thread]);
      num_thread++;
    }
  }
  for (int i = 0; i < num_thread; i++) {
    pthread_join(pool[i], NULL);
  }
}
