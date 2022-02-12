#define MAX_VECLEN 16777216 // 16MB

typedef unsigned int uint32_t;

static inline uint32_t rotate_left(uint32_t x, uint32_t n) {
    return  (x << n) | (x >> (32-n));
}

static inline uint32_t encrypt(uint32_t m, uint32_t key) {
    return (rotate_left(m, key&31) + key)^key;
}

__kernel void vecdot(int veclen,
                     uint32_t key1,
                     uint32_t key2,
                     __global uint32_t res[MAX_VECLEN]) {
  int globalIdx = get_global_id(0);
  int localIdx = get_local_id(0);
  res[globalIdx] = encrypt(globalIdx, key1) * encrypt(globalIdx, key2);
}

// vim: syntax=c sw=2
