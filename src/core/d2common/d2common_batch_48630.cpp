// D2Common 1.13c batch: stat-list damage-delta apply for a unit (item-type 0x2d "ethereal-like" gate).
// Callees are retail functions passed in (ponytail: no shared headers for their odd conventions):
//  c.rec(unit)=0x45A40 -> txt record, c.rnd(arg8,unit)=0x465B0, ITEMS_CheckItemTypeId(unit,0x2d)=0x24430 (direct),
//  c.chk(unit)=0x24C30, c.apply(p2,unit,p5,stat,0,delta,p9,p10)=0x475E0.
#include <cstdint>
#include <emscripten.h>

#include "D2Items.h"

extern "C" {
struct D2Common48630Callees {
 uint8_t* (*rec)(int32_t*);
 int32_t (*rnd)(int32_t, int32_t*);
 uint8_t (*chk)(int32_t*);
 void (*apply)(void*, int32_t*, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t);
};

// 1.13c D2Common+0x48630
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_48630(uint32_t, void* p2, int32_t* u, int32_t p4, int32_t p5, uint32_t, uint32_t,
                                                  int32_t p8, int32_t p9, uint32_t p10, const D2Common48630Callees* c) {
 if (p4 == 0) return 0;
 if (u == nullptr || *u != 4) return 0;
 uint8_t* r = c->rec(u);
 if (r == nullptr) return 0;
 if (p8 == 0) p8 = c->rnd(*(int32_t*)(intptr_t)(p4 + 8), u);
 auto clampd = [&](uint8_t b) { int32_t d = p8; if (b != 0 && (int32_t)(b + p8) < 1) d = -(int32_t)b; return d; };
 if (ITEMS_CheckItemTypeId(reinterpret_cast<const D2UnitStrc*>(u), 0x2d) == 0 || r[0xff] != 0 || r[0x103] == 0) {
  int32_t d = clampd(r[0xff]);
  if (d != 0) c->apply(p2, u, p5, 0x16, 0, d, p9, p10);
 }
 if (ITEMS_CheckItemTypeId(reinterpret_cast<const D2UnitStrc*>(u), 0x2d) == 0 || r[0x103] != 0 || r[0xff] == 0) {
  int32_t d = clampd(r[0x103]);
  if (d != 0) c->apply(p2, u, p5, 0x18, 0, d, p9, p10);
 }
 if (ITEMS_CheckItemTypeId(reinterpret_cast<const D2UnitStrc*>(u), 0x2d) != 0 && c->chk(u) == 0) return p8;
 int32_t d = clampd(r[0x101]);
 if (d != 0) c->apply(p2, u, p5, 0xa0, 0, d, p9, p10);
 return p8;
}
}
