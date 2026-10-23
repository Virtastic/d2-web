#include <stdint.h>
// 1.13c D2Client+0x3F590 : rect around (x,y) relative to scroll; caller passes it to D2Gfx 10070.
struct D2Rect16 { int32_t l, t, r, b; };
extern "C" D2Rect16 d2client_rect16_3f590(int32_t x, int32_t y, int32_t sx, int32_t sy) {
  D2Rect16 q; q.l = x - sx - 16; q.r = x - sx + 16; q.t = y - sy; q.b = q.t + 16; return q;
}
// 1.13c D2Client+0x3F360 : spinner dtor (vtable/gfx handle release; no pure logic)
// 1.13c D2Client+0x3F3D0 : spinner ctor (cel load through D2Gfx; )
