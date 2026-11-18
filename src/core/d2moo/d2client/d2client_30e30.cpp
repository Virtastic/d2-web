// Skill-description damage range from the unit's stat rows. Portions of the original 1.13c routine are table-driven.
#include "d2client_batch_30340.h"
namespace d2client_batch_30340 {
using UU = uint32_t;
// the percent term v*p/100 with the original's operand-size shapes
static I pctTerm(I v, I p) {
 if (v > 0x100000) { if ((v & ~0xf) < 0x640) return I(int64_t(p) * v / 100); return I(UU(v / 100) * UU(p)); }
 if (p > 0x10000) { if ((p & ~0xf) < 0x640) return I(int64_t(p) * v / 100); return I(UU(p / 100) * UU(v)); }
 return I(UU(p) * UU(v)) / 100;
}
// 1.13c D2Client+0x30E30
void sub_30E30(const Ops30E30& o, const Tables& t, P unit, I* mn, I* mx, I* mode, P flag) {
 for (int i = 0; i < 4; ++i) {
  I a = o.stat(unit, t.rows[i][0], 0), b = o.stat(unit, t.rows[i][1], 0);
  if (t.rows[i][2] != -1) { if (I p = o.stat(unit, t.rows[i][2], 0)) { a += pctTerm(a, p); b += pctTerm(b, p); } }
  if (!(a < b)) a = b;
  *mn += a; *mx += b; if (a || b) *mode = t.colors[i];
 }
 I lo = o.stat(unit, 0x39, 0), hi = o.stat(unit, 0x3a, 0);
 if (hi) {
  if (I p = o.stat(unit, 0x14c, 0)) { lo += pctTerm(lo, p); hi += pctTerm(hi, p); }
  I k = o.stat(unit, 0x65, 0);
  if (k <= 0) { I d = o.stat(unit, 0x146, 0); if (d <= 1) d = 1; k = o.stat(unit, 0x3b, 0) / d; }
  *mode = 2; *mn += I(UU(k) * UU(lo)) >> 8; *mx += I(UU(k) * UU(hi)) >> 8;
 }
 if (*mx > 0) { I m = *mn; if (m < 1) m = 1; *mn = m; I x = *mx; ++m; *mx = m > x ? m : x; }
 if (flag) { if (const uint8_t* r = o.unitRec(flag)) { if (r[4] & t.mask0c) o.f2980(unit, mn, mx, mode); } }
 if (o.o10263(unit)) *mode = 3;
 if (o.o10975(unit)) *mode = 1;
}
}
