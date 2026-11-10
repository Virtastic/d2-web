// 1.13c D2Client+0xA3580 over the FnCallees table. EAX = menu; plain ret. Lays the menu out: width = widest item text (0xD35A,
// ECX = item text) + 0x14, height = sum of item heights (+0x158) + 0xF, under font 1 (0xD3A8, ECX = font; the old font restored).
// Origin from the anchor (+0x24, +0x28), clamped to the screen size at 0xDBC48 / 0xDBC4C; fatal 0x5AA no items list (+0x58),
// 0x5DD too wide, 0x5E2 too tall. Then A32D0 per item.
#include "d2client_batch_a3040.h"
namespace d2client_batch_a3040 {
static inline int32_t& mA3580(uint32_t m, uint32_t o) { return fnAt<int32_t>(fnPtr(m), o); }
void sub_A3580(FnCallees& s, uint32_t m) {
 if (!mA3580(m, 0x58)) s.fatal(0x5aa);
 int32_t h = 0, w = 0, nul = 0;
 const uint32_t prev = s.h(0xd3a8, 0, 1);
 for (int32_t i = 0; i < mA3580(m, 0x50); ++i) {
  const uint32_t it = m + (uint32_t)i * 0x110;
  const int32_t t = (int32_t)s.h(0xd35a, 0, it + 0x68);
  if (t > w) w = t;
  if (!mA3580(it, 0x174)) ++nul;
  h += mA3580(it, 0x158);
 }
 w += 0x14; h += 0xf;
 s.h(0xd3a8, 0, prev);
 const int32_t x = mA3580(m, 0x24) - w / 2;
 mA3580(m, 0x2c) = x;
 mA3580(m, 0x30) = mA3580(m, 0x28) - (mA3580(m, 0x14) ? nul * 0x15 : h / 3);
 const int32_t sw = (int32_t)*s.g(0xdbc48);
 if (w > sw - 0x50) s.fatal(0x5dd);
 const int32_t sh = (int32_t)*s.g(0xdbc4c);
 if (h > sh - 0x62) s.fatal(0x5e2);
 if (x + w > sw - 10) mA3580(m, 0x2c) = sw - w;
 if (mA3580(m, 0x30) + h > sh - 0x3a) mA3580(m, 0x30) = (sh - h) - 0x30;
 if (mA3580(m, 0x2c) <= 10) mA3580(m, 0x2c) = 10;
 if (mA3580(m, 0x30) <= 10) mA3580(m, 0x30) = 10;
 mA3580(m, 0x34) = w; mA3580(m, 0x38) = h;
 for (int32_t i = 0; i < mA3580(m, 0x50); ++i) sub_A32D0(s, i, m);
}
}
