// D2Common 1.13c batch. Callees not yet wired are weak externs named by RVA.
#include <cstdint>
#include <emscripten.h>

extern "C" {
__attribute__((weak)) void d2_sub_CF40(uint32_t a, uint32_t b, int32_t x, int32_t y, int32_t* o1, uint32_t* o2);
__attribute__((weak)) void d2_sub_350B0(uint8_t* p);

// 1.13c D2Common+0x352F0: recompute path direction for p (fields +0,+4); store it at +0x64/+0x65 or fall to sub_350B0.
EMSCRIPTEN_KEEPALIVE void d2_sub_352F0(uint8_t* p, int32_t x, int32_t y, int32_t store) {
 if (!p) return;
 int32_t o1; uint32_t dir;
 d2_sub_CF40(*(uint32_t*)p, *(uint32_t*)(p + 4), x * 0x10000 + 0x8000, y * 0x10000 + 0x8000, &o1, &dir);
 if (store) { p[0x64] = dir & 0x3f; p[0x65] = dir & 0x3f; return; }
 d2_sub_350B0(p);
}
}
