// D2Common 1.13c 0x47120 (#10803, stdcall unit, skill, level, charges; ret 0x10): give an item (or the first item in the unit's
// inventory chain that takes it) the charged-skill stat 0xCC. Retail addresses are 32-bit words; the callees and sgptDataTables
// come in through the env (retail registers named on each seam).
#include <cstdint>
#include <cstring>
#include "d2common_47120.h"

static uint32_t rd(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Common+0x47120
int32_t D2Common_47120_AddChargedSkill(uint32_t unit, int32_t skill, uint32_t level, int32_t charges, const D2Common47120Env* e) {
 if (!unit || rd(unit) != 4) return 0;
 uint32_t t = e->dt();
 const uint32_t layer = ((uint32_t)skill << (rd(t + 0xc6c) & 0x1f)) + (rd(t + 0xc70) & level);
 const uint32_t par = rd(unit + 0x5c);
 if (par && (int32_t)rd(par + 0x10) < 0) {
  uint32_t n = e->find(par, 0x40);                      // 0x37F40: EAX = parent, ECX = 0x40
  while (n) {
   if ((int32_t)rd(t + 0xbd4) > 0xcc && rd(t + 0xbcc) + 0x10230 != 0) {
    const int32_t v = e->value(n, (0xccu << 16) + (layer & 0xffff), rd(t + 0xbcc) + 0x10230);   // 0x38B00: EBX list, EDI key, stack rec
    if (v) {
     const int32_t hi = v >> 8;
     if (hi < 1 || hi > 0xff) return 0;
     int32_t lo = charges & -(int32_t)(charges >= 1);
     if (lo >= hi) lo = hi;
     e->set(n, 0xcc, (uint32_t)(hi << 8) + ((uint32_t)lo & 0xff), layer, unit);   // 0x3A1D0 stdcall, 5 words
     return 1;
    }
    t = e->dt();
   }
   n = rd(n + 0x2c);
   while (n && !(rd(n + 0x10) & 0x40)) n = rd(n + 0x2c);
  }
 }
 const uint32_t inv = rd(unit + 0x60);
 if (!inv || rd(inv) != 0x1020304) return 0;
 for (uint32_t q = rd(inv + 0xc); q;) {
  if (D2Common_47120_AddChargedSkill(rd(q) == 4 ? q : 0, skill, level, charges, e)) return 1;
  if (rd(q) != 4) return 0;
  const uint32_t id = rd(q + 0x14);
  if (!id || id + 0x5c == 0) return 0;
  q = rd(id + 0x64);
 }
 return 0;
}
