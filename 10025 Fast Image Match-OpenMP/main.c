#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define NDEBUG

#ifndef NDEBUG
#define log(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#else
#define log(...)
#endif

struct int3tuple {
  int x, y, z;
};

static inline int cmp3tuple(struct int3tuple a, struct int3tuple b) {
  if (a.x < b.x)
    return -1;
  if (a.x > b.x)
    return 1;
  if (a.y < b.y)
    return -1;
  if (a.y > b.y)
    return 1;
  if (a.z < b.z)
    return -1;
  if (a.z > b.z)
    return 1;
  return 0;
}

#pragma omp declare reduction(min3tuple                                        \
                              : struct int3tuple                               \
                              : omp_out = cmp3tuple(omp_out, omp_in) > 0 ?     \
                                          omp_in : omp_out)                    \
                    initializer(omp_priv = (struct int3tuple) {1e9, -1, -1})

uint8_t A[512][512];
uint8_t B[512][512];
int AH, AW, BH, BW;

int main() {
  while (scanf("%d%d%d%d", &AH, &AW, &BH, &BW) == 4) {
    for (int i = 0; i < AH; i++) {
      for (int j = 0; j < AW; j++) {
        scanf("%hhu", &A[i][j]);
      }
    }
    for (int i = 0; i < BH; i++) {
      for (int j = 0; j < BW; j++) {
        scanf("%hhu", &B[i][j]);
      }
    }

    // {min_diff, best_x, best_y}
    struct int3tuple best = {1e9, -1, -1};
#pragma omp parallel for reduction(min3tuple : best)
    for (int si = 0; si < AH - BH + 1; si++) {
      for (int sj = 0; sj < AW - BW + 1; sj++) {
        int diff = 0;
        for (int i = 0; i < BH; i++) {
          for (int j = 0; j < BW; j++) {
            int a = A[si + i][sj + j];
            int b = B[i][j];
            diff += (a - b) * (a - b);
          }
        }

        log("si=%d sj=%d diff=%d best_x=%d best_y=%d min_diff=%d", si, sj, diff,
            best.y, best.z, best.x);

        struct int3tuple cand = {diff, si, sj};
        if (cmp3tuple(cand, best) < 0) {
          best = cand;
        }
      }
    }
    printf("%d %d\n", best.y + 1, best.z + 1);
  }
}

// vim: sw=2
