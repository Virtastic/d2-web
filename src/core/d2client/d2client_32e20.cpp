// Skill-description percent scaling pass: scales the pending damage range by the skill row's percent.
#include "d2client_32e20.h"
#include <cstring>
namespace d2client_32e20 {
static inline I scale(I pct, I v) { return v + I(U(pct) * U(v)) / 100; }
// 1.13c D2Client+0x32E20  (ecx=unit, edx=flag; stack a0..a4, a0 only tested; a1 unused)
void sub_32E20(const Ops& o, const uint8_t* dt, U unit, U flag, U a0, U a2, U a3, U a4) {
 if (!flag || !a0) return;
 I mn = 0, mx = 0, mode = 0;
 o.o31220(unit, flag, &mn, &mx, &mode);
 const I id = o.oC42A(flag, 0xD6990, 0xD40);
 I pct = 0;
 I cnt; memcpy(&cnt, dt + 0xBA0, 4);
 if (id >= 0 && id < cnt) {
  U base; memcpy(&base, dt + 0xB98, 4);
  const U row = base + U(id) * 0x23C;
  if (row) memcpy(&pct, (const uint8_t*)(uintptr_t)row + 0x158, 4);
 }
 mn = scale(pct, mn);
 mx = scale(pct, mx);
 o.o30E30(unit, &mn, &mx, &mode, flag);
 o.emit(mx, mn, mode, I(a2), I(a3), I(a4));
}
}
