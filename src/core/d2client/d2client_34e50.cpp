// Skill-description bonus range: item and stat percent scaling over three record fields.
#include "d2client_34e50.h"
#include <cstring>
namespace d2client_34e50 {
static inline I mul32(I a, I b) { return I(U(a) * U(b)); }
// D2Client+0x14A0 (eax=pct, edx=v, ecx=d)
static I mulDiv(I pct, I v, I d) {
 if (!d) return 0;
 if (v <= 0x100000) { if (pct <= 0x10000) return mul32(v, pct) / d; if (d <= (pct >> 4)) return (pct / d) * v; }
 else if (d <= (v >> 4)) return (v / d) * pct;
 return I((int64_t(v) * pct) / d);
}
static U rd32(const uint8_t* p) { U v; memcpy(&v, p, 4); return v; }
// 1.13c D2Client+0x34E50
void sub_34E50(const Ops& o, P unit, U a, const uint8_t* rec, I b, U w2, U w3, U w4) {
 if (!a || !rec) return;
 U mode = o.o2C830(rec[0x1DC]);
 const U scale = rec[0x1A5];
 I x = 0, y = 0;
 o.o34D70(&mode, 0, scale, a, unit, &x, &y);
 I ebp, edi;
 P own = nullptr;
 bool two = false;
 if (o.oC6EE(unit, 1)) { memcpy(&own, (const uint8_t*)unit + 0x60, 4); two = o.oC89E(unit, own) == 2; }
 if (two) { ebp = o.stat(unit, 0x17, 0); edi = o.stat(unit, 0x18, 0); }
 else { ebp = o.stat(unit, 0x15, 0); edi = o.stat(unit, 0x16, 0); }
 if (ebp <= 1) ebp = 1;
 if (edi <= 2) edi = 2;
 if (edi < ebp) edi = ebp;
 const I id = (int16_t)(rec[0] | (rec[1] << 8));
 static const int fld[3] = {0x138, 0x13C, 0x140};
 static const I st[3] = {0x149, 0x14B, 0x14A};
 for (int i = 0; i < 3; i++) {
  I esi = o.oC316(unit, rd32(rec + fld[i]), id, b);
  if (esi) { const I p = o.stat(unit, st[i], 0); if (p) esi += mulDiv(p, esi, 100); }
  x += mulDiv(esi, ebp, 100);
  y += mulDiv(esi, edi, 100);
 }
 if (!mode) mode = 3;
 o.emit(y, x, mode, w2, w3, w4);
}
}
