// D2Common 1.13c batch. Callees are seams (retail addresses); fatal paths trap (retail: _exit(-1)).
#include <cstdint>
#include <emscripten.h>

struct D2Env496F0 {
 int32_t (*f464D0)(int32_t a, int32_t* p);                   // 0x464D0
 int32_t (*itemsTxt)(int32_t id);                            // DATATBLS_GetItemsTxtRecord
 void (*ord10213)(void* tbl, int32_t v, int32_t z);          // Ordinal_10213 on DAT_6fdeff6c
 void* tbl10213;
 uint32_t* (*f47020)(void* a, int32_t b, uint32_t c);        // 0x47020
 int32_t (*f38B00)(int32_t p);                               // 0x38B00
 void (*f3A330)(uint32_t* p, int32_t a, int32_t b);          // 0x3A330
 const uint8_t* dataTables;                                  // sgptDataTables
};

extern "C" {
// 1.13c D2Common+0x496F0
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_496f0(const D2Env496F0* e, int32_t p1, void* p2, int32_t* p3,
                                                   int32_t p4, uint32_t p5, int32_t p6, int32_t p7, uint32_t p8) {
 int32_t r = e->f464D0(*reinterpret_cast<int32_t*>(p4 + 8), p3);
 if (p1 == 1) {
  if (!p3 || *p3 != 4) __builtin_trap();
  int32_t rec = e->itemsTxt(p3[1]);
  if (!rec) __builtin_trap();
  e->ord10213(e->tbl10213, *reinterpret_cast<int32_t*>(rec + 0x80), 0);
 }
 if (r) {
  uint32_t* u = e->f47020(p2, p7, p8);
  if (!u) __builtin_trap();
  if (p6) {
   const int32_t cnt = *reinterpret_cast<const int32_t*>(e->dataTables + 0xbd4);
   const int32_t base = *reinterpret_cast<const int32_t*>(e->dataTables + 0xbcc) + 0xa0bc;
   r = -((cnt < 0x80 || !base) ? 0 : e->f38B00(base));
  }
  e->f3A330(u, 0x7f, r);
 }
 return p5;
}
}
