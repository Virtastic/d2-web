#include <stdint.h>
// 1.13c D2Client+0x3F590 (EAX = x, stack y, colour; ret 8): the 32x16 box {x-sx-16, y-sy, x-sx+16, y-sy+16}
// around a point, scroll origin sx = [0x119960], sy = [0x11995C], drawn through D2Gfx 10070(&box, colour).
extern "C" __attribute__((used, visibility("default")))
void d2_client_3f590(int32_t x, int32_t y, int32_t colour, int32_t sx, int32_t sy,
                     void (*drawRect)(const int32_t* box, int32_t colour)) {
  const int32_t l = x - sx, t = y - sy;
  const int32_t box[4] = { l - 16, t, l + 16, t + 16 };
  drawRect(box, colour);
}
// dup of D2CLIENT+0x3F360 : spinner dtor (vtable/gfx handle release; no pure logic)
// 1.13c D2Client+0x3F3D0 : spinner ctor (cel load through D2Gfx; )
