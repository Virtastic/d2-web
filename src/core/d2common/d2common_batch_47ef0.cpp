// D2Common 1.13c 0x47EF0: apply (or revoke) a stat delta on a child stat list of an item. Registers: EAX gate (nonzero, or
// mode == 1, runs 0x46E50), ECX unit, EDI stat; stack mode, self, value, revoke, id, filter (ret 0x18).
#include <cstdint>
#include "d2common_47ef0.h"

// 1.13c D2Common+0x47EF0
uint32_t D2Common_47EF0_ApplyStat(uint32_t gate, uint32_t unit, uint32_t stat, int32_t mode, uint32_t self, int32_t value,
                                  int32_t revoke, uint32_t id, uint32_t flt, const D2Common47EF0Env* e) {
 if (gate != 0 || mode == 1) e->apply(unit, stat);
 if (value == 0) return 0;
 e->shift(stat, &value);
 const uint32_t child = e->child(self, unit, id, flt);
 if (!child) { e->fatal(0x2c5); __builtin_trap(); }   // retail: _exit(-1)
 if (revoke) { e->add(child, stat, -e->get(child, stat, 0), 0); return 1; }
 e->add(child, stat, value, 0);
 return 1;
}
