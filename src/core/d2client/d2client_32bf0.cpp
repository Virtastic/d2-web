// Skill-description damage range for the skill's own tooltip line: base damage plus bonus passes, then the emit sink.
#include "d2client_batch_30340.h"
namespace d2client_batch_30340 {
// D2Client+0x14A0 (eax=pct, edx=v, ecx=d)
static I mulDiv(I pct, I v, I d) {
 if (!d) return 0;
 if (v <= 0x100000) { if (pct <= 0x10000) return (v * pct) / d; if (d <= (pct >> 4)) return (pct / d) * v; }
 else if (d <= (v >> 4)) return (v / d) * pct;
 return I((int64_t(v) * pct) / d);
}
static bool usable(const Ops32BF0& o, P s) { return o.o10744(s, 0x2d) && o.o11006(s) && o.o11160(s, 0); }
// 1.13c D2Client+0x32BF0  (ecx=unit, edx=flag; stack sk,lvl,a2,a3,a4)
void sub_32BF0(const Ops32BF0& o, P unit, I flag, const uint8_t* sk, I lvl, I a2, I a3, I a4) {
 if (!flag || !sk) return;
 int16_t sk16; memcpy(&sk16, sk, 2); const I skill = sk16;
 I mode = o.o2C830(sk[0x1dc]);
 const I A = o.dmgMin(unit, skill, lvl, 0) >> 8;
 const I B = o.dmgMax(unit, skill, lvl, 0) >> 8;
 const I bon = o.bonusTotal(); I pct = bon, x = 0, y = 0;
 o.calc10323(unit, &x, &y, &pct);
 x = x + A + mulDiv(pct, x, 100) + mulDiv(bon, A, 100);
 y = y + B + mulDiv(pct, y, 100) + mulDiv(bon, B, 100);
 int32_t w138; memcpy(&w138, sk + 0x138, 4);
 const I p = o.o10786(unit, w138, skill, lvl);
 x += mulDiv(p, x, 100); y += mulDiv(p, y, 100);
 if (P own = o.owner(unit)) {
  P s = o.skillOf(own), w = nullptr;
  if (s) w = o.o11139(own, (o.o10417(s) == 4) + 4);
  if (usable(o, s)) o.o10164(unit, s, 0); else s = nullptr;
  if (w && w != s && usable(o, w)) o.o10164(unit, w, 0);
  o.o30E30(unit, &x, &y, &mode, flag);
  if (s) o.o10164(unit, s, 1);
 }
 o.emit(y, x, mode, a2, a3, a4);
}
}
