// The skill-description damage range with an elemental-only base: ((SrcShift-scaled base) + the unit's elemental min/max) * 75 / 256,
// then the 0x34D70 bonus pass, then the draw (0x2F0A0).
#include "d2client_35140.h"
namespace d2client_35140 {
static inline I mul32(I a, I b) { return I(U(a) * U(b)); }
static inline U rd32(const uint8_t* p) { return U(p[0]) | (U(p[1]) << 8) | (U(p[2]) << 16) | (U(p[3]) << 24); }
// 1.13c D2Client+0x35070
void sub_35070(const Ops& o, P unit, U flag, const uint8_t* rec, I lvl, U w2, U w3, U w4) {
 if (!flag || !rec) return;
 const U sh = rec[0x1A4] & 31;
 U lo = rd32(rec + 0x1A8) << sh;
 U hi = rd32(rec + 0x1AC) << sh;
 const I id = int16_t(uint16_t(rec[0] | (rec[1] << 8)));
 lo += U(o.eMin(unit, id, lvl, 1));
 hi += U(o.eMax(unit, id, lvl, 1));
 U mode = o.o2C830(rec[0x1DC]);
 I x = mul32(I(lo), 0x4b) >> 8;
 I y = mul32(I(hi), 0x4b) >> 8;
 o.o34D70(&mode, 0, rec[0x1A5], flag, unit, &x, &y);
 o.emit(y, x, mode, w2, w3, w4);
}
}
