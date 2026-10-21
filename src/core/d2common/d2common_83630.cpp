// 1.13c D2Common+0x83630: for each node of the list at (param_3+0x50)->+0x54 (next at +0xc) whose
// position (+8, +0x18) lies strictly inside the rect (EAX = {x,y,w,h} in tiles, *5 to sub-units),
// call sub_44480 with (node+0x14, node+4). The 44480 call is a callback seam (register args, EAX = rect).
#include <cstdint>
#include <emscripten.h>
namespace { int32_t rd(uint32_t a, int o) { return *(int32_t*)(uintptr_t)(a + o); } }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_83630(const int32_t* rect, uint32_t ctx, void (*f44480)(uint32_t, uint32_t)) {
  int32_t x = rect[0], y = rect[1], w = rect[2], h = rect[3];
  for (uint32_t n = rd(rd(ctx, 0x50), 0x54); n; n = rd(n, 0xc))
    if (x * 5 < rd(n, 8) && y * 5 < rd(n, 0x18) && rd(n, 8) < w * 5 + x * 5 && rd(n, 0x18) < h * 5 + y * 5)
      f44480(rd(n, 0x14), rd(n, 4));
}
