// Skill-description damage range for a unit's skill or item: weapon damage for a weapon, otherwise stat bonuses.
#include "d2client_32f30.h"
namespace d2client_32f30 {
static inline I mul32(I a, I b) { return I(U(a) * U(b)); }
static inline I div100(I v) { return v / 100; }
// D2Client+0x14A0 (eax=pct, edx=v, ecx=d)
static I mulDiv(I pct, I v, I d) {
 if (!d) return 0;
 if (v <= 0x100000) { if (pct <= 0x10000) return mul32(v, pct) / d; if (d <= (pct >> 4)) return (pct / d) * v; }
 else if (d <= (v >> 4)) return (v / d) * pct;
 return I((int64_t(v) * pct) / d);
}
// 1.13c D2Client+0x32F30
void sub_32F30(const Ops& o, P unit, I* pMin, I* pMax, I* pMode, I a2, U skill, P item) {
 *pMin = 0; *pMax = 0;
 if (!skill) return;
 if (!item) {
  P own = o.owner(unit);
  if (!own) return;
  item = o.skillOf(own);
  if (!item) return;
 }
 if (!o.oC76C(item)) return;
 if (o.oC1CC(item, 0x26)) {
  *pMode = 0;
  const U e = o.oC736(item);
  I mn = o.oC880(0, unit, e, 1);
  mn += o.oC8E0(0, unit, e, 1);
  I mx = o.oC934(0, unit, e, 1);
  mx += o.oC96A(0, unit, e, 1);
  switch (o.oC868(e)) {
   case 1: *pMode = 1; break;
   case 2: *pMode = 4; break;
   case 4: *pMode = 3; break;
   case 5: {
    *pMode = 2;
    I d = o.oC832(0, unit, e, 1) / 25;
    if (d <= 0) d = 1;
    mn /= d; mx /= d;
    break;
   }
   default: break;
  }
  mn >>= 8; mx >>= 8;
  if (mx < mn) mx = mn;
  *pMin = mn; *pMax = mx;
  return;
 }
 I edi = 0;
 const I pa = o.oC940(item);
 if (pa) edi = div100(mul32(o.stat(unit, 0, 0), pa));
 const I pb = o.oC8CE(item);
 if (pb) edi += div100(mul32(o.stat(unit, 2, 0), pb));
 *pMin = o.stat(unit, 0x9F, 0);
 *pMax = o.stat(unit, 0xA0, 0);
 edi += o.stat(unit, 0x19, 0);
 const I s12 = o.stat(unit, 0x12, 0);
 const I s11 = o.stat(unit, 0x11, 0);
 if (edi <= -0x5A) edi = -0x5A;
 *pMin = *pMin + div100(mul32(s12 + edi, *pMin));
 *pMax = *pMax + div100(mul32(s11 + edi, *pMax));
 edi = a2;
 if (o.oC1FC(unit, 0x4E)) edi = o.oC922(unit, item, skill, 1) + a2;
 *pMin += mulDiv(edi, *pMin, 100);
 *pMax += mulDiv(edi, *pMax, 100);
 if (*pMax < *pMin) *pMax = *pMin;
 I mnx = 0, mxx = 0;
 o.o30E30(unit, &mnx, &mxx, pMode, skill);
 const I e2 = mxx < mnx ? mnx : mxx;
 *pMin += mnx; *pMax += e2;
}
}
