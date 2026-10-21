// D2Common 1.13c batch. Path step state p (u32 words): [0]=cur x|y<<16, [1]=target x|y<<16,
// [2],[3]=dir vector, [4],[5]=scaled step, [6]=speed, [7] byte=direction, [8]=flags.
#include <cstdint>
#include <emscripten.h>
#include "Path/Step.h"

extern "C" {
// 1.13c D2Common+0x3E400: recompute step vector toward target; 0 when the step overshoots, else 1.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_3e400(uint32_t* p) {
 if ((uint16_t)p[0] == (uint16_t)p[1] && (uint16_t)(p[0] >> 16) == (uint16_t)(p[1] >> 16)) {
  p[2] = p[3] = p[4] = p[5] = p[6] = 0;
  return 1;
 }
 D2CoordStrc v = {0, 0}; int d = 0;
 const uint32_t c = p[0], t = p[1];
 PATH_GetDirectionVector(&v, &d, (int32_t)(c * 0x10000u + 0x8000u), (int32_t)((c & 0xffff0000u) + 0x8000u),
                         (int32_t)(t * 0x10000u + 0x8000u), (int32_t)((t & 0xffff0000u) + 0x8000u));
 p[2] = (uint32_t)v.nX; p[3] = (uint32_t)v.nY;
 *(uint8_t*)&p[7] = (uint8_t)d;
 if (p[8] & 0x200) *(uint8_t*)&p[7] = (uint8_t)((d - 0x20) & 0x3f);
 int32_t sx = (int32_t)(p[2] * p[6]) >> 8, sy = (int32_t)(p[3] * p[6]) >> 8;
 p[4] = (uint32_t)sx; p[5] = (uint32_t)sy;
 int32_t ax = sx < 0 ? -sx : sx, ay = sy < 0 ? -sy : sy;
 int32_t dx = (int32_t)(uint16_t)p[0] - (int32_t)(uint16_t)p[1]; if (dx < 0) dx = -dx;
 int32_t dy = (int32_t)(uint16_t)(p[0] >> 16) - (int32_t)(uint16_t)(p[1] >> 16); if (dy < 0) dy = -dy;
 return (dx * 0x10000 < ax && dy * 0x10000 < ay) ? 0 : 1;
}
}
