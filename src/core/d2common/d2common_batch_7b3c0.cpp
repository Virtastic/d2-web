// D2Common 1.13c batch: A* all-8-neighbour expansion. chk = D2Common+0x4CE90, ev = D2Common+0x7AFB0
// (passed in, as in d2common_batch_186d0.cpp). Args: p = EAX point (u16 x,y), ctx = param_1.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x7B3C0: evaluate each of 8 neighbours of *p that is not blocked; 0 if any evaluation fails.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_7b3c0(const uint16_t* p, const int32_t* ctx, int32_t cur, uint32_t tgt,
  uint32_t (*chk)(int32_t, uint32_t, uint32_t, int32_t, uint32_t),
  int32_t (*ev)(const int32_t*, int32_t, uint32_t, uint32_t)) {
 const int32_t v8 = ctx[2], v28 = ctx[10];
 const uint32_t v2c = static_cast<uint32_t>(ctx[11]);
 static const int8_t d[8][2] = {{-1,-1},{-1,1},{1,-1},{1,1},{-1,0},{0,-1},{1,0},{0,1}};
 for (int i = 0; i < 8; i++) {
  const uint16_t x = static_cast<uint16_t>(p[0] + d[i][0]), y = static_cast<uint16_t>(p[1] + d[i][1]);
  if (chk(v8, x, y, v28, v2c) == 0 && ev(ctx, cur, static_cast<uint32_t>(x) | (static_cast<uint32_t>(y) << 16), tgt) == 0) return 0;
 }
 return 1;
}
}
