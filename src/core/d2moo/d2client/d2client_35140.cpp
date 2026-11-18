// Skill-description damage range: base min/max, the 0x34D70 bonus pass over them, then the elemental (or poison-length scaled) terms.
#include "d2client_35140.h"
namespace d2client_35140 {
static inline I mul32(I a, I b) { return I(U(a) * U(b)); }
// 1.13c D2Client+0x35140
void sub_35140(const Ops& o, P unit, U flag, const uint8_t* rec, I lvl, U w2, U w3, U w4) {
 if (!flag || !rec) return;
 const U scale = rec[0x1A5];
 U mode = o.o2C830(rec[0x1DC]);
 const I id = int16_t(uint16_t(rec[0] | (rec[1] << 8)));
 I x = o.dmgMin(unit, id, lvl, 0) >> 8;
 I y = o.dmgMax(unit, id, lvl, 0) >> 8;
 o.o34D70(&mode, 0, scale, flag, unit, &x, &y);
 I hi;
 if (rec[0x1DC] == 5) {   // poison: the elemental terms are per-frame, scaled by the length
  const I k = o.len(unit, id, lvl, 1);
  x += mul32(o.eMin(unit, id, lvl, 1), k) >> 8;
  hi = mul32(o.eMax(unit, id, lvl, 1), k);
 } else {
  x += o.eMin(unit, id, lvl, 1) >> 8;
  hi = o.eMax(unit, id, lvl, 1);
 }
 y += hi >> 8;
 o.emit(y, x, mode, w2, w3, w4);
}
}
