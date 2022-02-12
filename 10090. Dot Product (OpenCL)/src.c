#include <stdio.h>
#include <assert.h>
#include <omp.h>
#include <inttypes.h>
#include "utils.h"

#define NDEBUG

#ifndef NDEBUG
#define logdbg(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#else
#define logdbg(...)
#endif

int main(int argc, char *argv[]) {
    int N;
    uint32_t key1, key2;
    while (scanf("%d %" PRIu32 " %" PRIu32, &N, &key1, &key2) == 3) {
        uint32_t sum = 0;
#pragma omp parallel for schedule(static) reduction(+: sum)
        for (int i = 0; i < N; i++) {
            uint32_t product = encrypt(i, key1) * encrypt(i, key2);
            sum += product;
            logdbg("i=%d product=%d", i, product);
        }

        printf("%" PRIu32 "\n", sum);
    }
    return 0;
}
