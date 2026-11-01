// Item-description tooltip tail: either the full line set (flag) or the fade-range line.
#include "d2client_30270.h"
namespace d2client_30270 {
// 1.13c D2Client+0x30270  (eax=edi, esi; stack a0,a1,a2; always returns 1)
U sub_30270(const Ops& o, bool flag, U edi, U esi, U a0, U a1, U a2) {
 if (flag) {
  const U r = o.o2F2E0(edi, a1, esi, a0, a2);
  o.o2EF80(r, a1, a0, esi, a0, a2);
  o.o2D000(edi, 0, a0, esi, a0, a1);
  const U v = o.oCEAA(esi, a1, edi);
  if (v) o.o2D9B0(v, a0, v);
  return 1;
 }
 I mn = 0, mx = 0, mode = 0;
 o.o2E190(a2, edi, esi, &mn, &mx, &mode);
 o.o2F520(a0, mn, mx);
 return 1;
}
}
