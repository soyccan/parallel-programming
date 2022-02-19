typedef unsigned int uint32_t;

static inline uint32_t rotate_left(uint32_t x, uint32_t n) {
  return (x << n) | (x >> (32 - n));
}

static inline uint32_t encrypt(uint32_t m, uint32_t key) {
  return (rotate_left(m, key & 31) + key) ^ key;
}

__kernel void vecdot0(uint32_t key1, uint32_t key2, __global uint32_t *res) {
  size_t gid = get_global_id(0);
  size_t gsz = get_global_size(0);

  res[gid] = encrypt(gid, key1) * encrypt(gid, key2);

  for (size_t s = 1; s < gsz; s *= 2) {
    barrier(CLK_GLOBAL_MEM_FENCE);
    if (gid % (2 * s) == 0) {
      res[gid] += res[gid + s];
    }
  }
}

// reduce divergent warps, remove modulo operation
__kernel void vecdot1(uint32_t key1, uint32_t key2, __global uint32_t *res) {
  size_t gid = get_global_id(0);
  size_t gsz = get_global_size(0);

  res[gid] = encrypt(gid, key1) * encrypt(gid, key2);

  for (size_t s = 1; s < gsz; s *= 2) {
    barrier(CLK_GLOBAL_MEM_FENCE);
    size_t idx = 2 * s * gid; // note overflow
    if (idx < gsz) {
      res[idx] += res[idx + s];
    }
  }
}

// reduce memory bank conflicts by changing interleaved addressing to sequential
// addressing
__kernel void vecdot2(uint32_t key1, uint32_t key2, __global uint32_t *res) {
  size_t gid = get_global_id(0);
  size_t gsz = get_global_size(0);

  res[gid] = encrypt(gid, key1) * encrypt(gid, key2);

  for (size_t s = gsz / 2; s > 0; s /= 2) {
    barrier(CLK_GLOBAL_MEM_FENCE);
    if (gid < s) {
      res[gid] += res[gid + s];
    }
  }
}

// unroll last warp
__kernel void vecdot3(uint32_t key1, uint32_t key2,
                      __global volatile uint32_t *res) {
  size_t gid = get_global_id(0);
  size_t gsz = get_global_size(0);

  res[gid] = encrypt(gid, key1) * encrypt(gid, key2);

  for (size_t s = gsz / 2; s > 32; s /= 2) {
    barrier(CLK_GLOBAL_MEM_FENCE);
    if (gid < s) {
      res[gid] += res[gid + s];
    }
  }

  if (gid < 32) {
    // warp reduction
    res[gid] += res[gid + 32];
    res[gid] += res[gid + 16];
    res[gid] += res[gid + 8];
    res[gid] += res[gid + 4];
    res[gid] += res[gid + 2];
    res[gid] += res[gid + 1];
  }
}

// vim: syntax=c sw=2
