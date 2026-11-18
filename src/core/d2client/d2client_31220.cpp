// Skill-description bonus pass: folds the unit's skill and stat bonuses into a pending damage range.
#include "d2client_batch_30340.h"
namespace d2client_batch_30340 {
using UU = uint32_t;
static inline I mul32(I a, I b) { return I(UU(a) * UU(b)); }
static inline I div100(I v) { return v / 100; }
// 1.13c D2Client+0x31220  (eax=base, ecx=unit; stack mn,mx,mode,a3,flag,scale,sk,useSkill; returns 1 when the range was updated)
I sub_31220(const Ops31220& o, const Tables& t, I base, P unit, I* mn, I* mx, I* mode, I a3, P flag, I scale, P sk, I useSkill) {
 if (!flag) return 0;
 const uint8_t* rec = o.rec(flag);
 if (!rec) return 0;
 P own = o.owner(unit);
 if (!own) return 0;
 if (!sk) sk = o.skillOf(own);
 I lo, hi;
 if (sk) {
  { I w; memcpy(&w, (const uint8_t*)sk + 4, 4); o.c1ba(w); }
  if (useSkill) { lo = o.skStat(unit, sk, 0x15); hi = o.skStat(unit, sk, 0x16); }
  else if (o.mode(unit, own) == 2) { lo = o.stat(unit, 0x17); hi = o.stat(unit, 0x18); }
  else { lo = o.stat(unit, 0x15); hi = o.stat(unit, 0x16); }
 } else { lo = o.stat(unit, 0x15) + 1; hi = o.stat(unit, 0x16) + 2; }
 if (!scale) scale = rec[0x1a5];
 if (scale != 0x80) {
  I p = mul32(lo, scale); lo = I(p + I(UU(p >> 31) & 0x7f)) >> 7;
  p = mul32(hi, scale); hi = I(p + I(UU(p >> 31) & 0x7f)) >> 7;
 }
 if (scale) { if (lo < 1) lo = 1; *mn += lo; if (hi < 2) hi = 2; *mx += hi; }
 base += o.stat(unit, 0x19);
 base += o.o10804(unit, sk, flag, 1);
 const I s12 = o.stat(unit, 0x12), s11 = o.stat(unit, 0x11);
 if (t.mask0c & rec[4]) { if (I f = o.bonusTotal()) { base += f; *mode = 3; } }
 I edi = 0;
 if (sk) {
  if (I p = o.pct1(sk)) base += div100(mul32(o.stat(unit, 0), p));
  if (I p = o.pct2(sk)) base += div100(mul32(o.stat(unit, 2), p));
  edi = o.skval(sk, 0x6f, 0);
 } else base += o.stat(unit, 0);
 if (o.o10263(unit)) *mode = 3;
 if (o.o10975(unit)) *mode = 1;
 if (base <= -0x5a) base = -0x5a;
 *mn = div100(mul32(s12 + base + 100, *mn)) + edi + a3;
 *mx = div100(mul32(s11 + base + 100, *mx)) + edi + a3;
 return 1;
}
}
