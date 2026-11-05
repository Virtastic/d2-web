// D2Common 1.13c 0x47FA0: resolve a property value (0x463F0 when EAX != 0, else 0x464D0, on [prop+8] / [prop+0xC] and the item),
// then add (or revoke) it on a child stat list; stat 0x3A also adds 1 to stat 0x146. Registers: EAX gate, ECX prop, EDX unit,
// EDI stat; stack mode, self, revoke, id, filter (ret 0x14).
#include <cstdint>
#include <cstring>
#include "d2common_47ef0.h"

static uint32_t rd(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Common+0x47FA0
uint32_t D2Common_47FA0_ApplyResolvedStat(uint32_t gate, uint32_t prop, uint32_t unit, uint32_t stat, int32_t mode, uint32_t self,
                                          int32_t revoke, uint32_t id, uint32_t flt, const D2Common47EF0Env* e) {
 int32_t v;
 if (gate != 0) { v = e->resolve463f0(rd(prop + 8), rd(prop + 0xc), unit); e->apply(unit, stat); }
 else { v = e->resolve464d0(rd(prop + 8), rd(prop + 0xc), unit); if (mode == 1) e->apply(unit, stat); }
 if (v == 0) return 0;
 e->shift(stat, &v);
 const uint32_t child = e->child(self, unit, id, flt);
 if (!child) { e->fatal(0x29d); __builtin_trap(); }   // retail: _exit(-1)
 e->add(child, stat, revoke ? -e->get(child, stat, 0) : v, 0);
 if (stat == 0x3a) e->add(child, 0x146, 1, 0);
 return 1;
}
