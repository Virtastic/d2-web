// D2Common 1.13c batch. Retail: ctx = EAX (flag at +0x36fd0, seed ptr at +0x36fd4), s = ESI
// (+0xc -> int source, +0x10 accumulator), tbl = DAT_6fddb960 (32 ints). Passed in as args.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x707E0: advance accumulator s+0x10 by *src (+ table[seed&31] when seeded), mod 8.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_707e0(int32_t flag, uint32_t* seed, const int32_t* tbl, const int32_t* src, uint32_t* acc) {
 if (flag) {
  const uint64_t v = (uint64_t)seed[0] * 0x6ac690c5u + seed[1];
  seed[0] = (uint32_t)v; seed[1] = (uint32_t)(v >> 32);
  *acc = (uint32_t)(tbl[(uint32_t)v & 0x1f] + *src + (int32_t)*acc) & 7;
  return;
 }
 *acc = (uint32_t)(*src + (int32_t)*acc) & 7;
}
}
