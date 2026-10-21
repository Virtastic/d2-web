// D2Common 1.13c batch: apply the four per-item stat hooks to a room-side target.
// Callees not yet wired are weak externs named by RVA / ordinal; retail _exit(-1) is a trap.
#include <cstdint>
#include <emscripten.h>

extern "C" {
__attribute__((weak)) int32_t D2Common_464D0(int32_t a, int32_t* unit);
__attribute__((weak)) void D2Common_47EF0(int32_t p1, void* p2, int32_t v, int32_t p6, int32_t p7, uint32_t p8);
__attribute__((weak)) uint32_t* D2Common_47020(void* p2, int32_t p7, uint32_t p8);
__attribute__((weak)) void D2Common_3A330(uint32_t* p, int32_t stat, int32_t v);
__attribute__((weak)) int32_t D2Common_38B00(int32_t rec);
__attribute__((weak)) intptr_t DATATBLS_GetItemsTxtRecord(int32_t id);
__attribute__((weak)) int32_t D2Common_Ord10265(int32_t);
__attribute__((weak)) void D2Common_Ord10024(const void*, int32_t);
__attribute__((weak)) void D2Common_Ord10213(void*, int32_t, int32_t);
__attribute__((weak)) uint8_t* sgptDataTables;
__attribute__((weak)) void* DAT_6fdeff6c;
__attribute__((weak)) char DAT_6fdda728[1];

[[noreturn]] static void fail(int32_t code) {
 D2Common_Ord10024(DAT_6fdda728, D2Common_Ord10265(code));
 __builtin_trap();
}

// 1.13c D2Common+0x49F30
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_49f30(int32_t p1, void* p2, int32_t* unit, int32_t p4, int32_t p5, int32_t p6, int32_t p7, uint32_t p8) {
 if (!unit) fail(0x1505);
 if (unit[0] != 4) fail(0x1506);
 if (!unit[5]) fail(0x1508);
 const bool hasExtra = *reinterpret_cast<int16_t*>(static_cast<intptr_t>(unit[5]) + 0x30) != 0;
 if (hasExtra) {
  if (!p4) fail(0x349);
  const int32_t v = D2Common_464D0(*reinterpret_cast<int32_t*>(static_cast<intptr_t>(p4) + 8), unit);
  for (int i = 0; i < 4; ++i) D2Common_47EF0(p1, p2, v, p6, p7, p8);
  return p5;
 }
 static const int32_t off[4] = {0x315c, 0x33e4, 0x366c, 0x38f4};
 static const int32_t stat[4] = {0x27, 0x29, 0x2b, 0x2d};
 static const int32_t need[4] = {0x28, 0x2a, 0x2c, 0x2e};
 for (int i = 0; i < 4; ++i) {
  int32_t v = D2Common_464D0(*reinterpret_cast<int32_t*>(static_cast<intptr_t>(p4) + 8), unit);
  if (p1 == 1) {
   if (unit[0] != 4) fail(0x61c);
   const intptr_t row = DATATBLS_GetItemsTxtRecord(unit[1]);
   if (!row) fail(0x61f);
   D2Common_Ord10213(DAT_6fdeff6c, *reinterpret_cast<int32_t*>(row + 0x80), 0);
  }
  if (!v) continue;
  uint32_t* h = D2Common_47020(p2, p7, p8);
  if (!h) fail(0x29d);
  if (p6) {
   uint8_t* t = sgptDataTables;
   const int32_t base = *reinterpret_cast<int32_t*>(t + 0xbcc);
   v = (*reinterpret_cast<int32_t*>(t + 0xbd4) < need[i] || !(base + off[i])) ? 0 : D2Common_38B00(base + off[i]);
   v = -v;
  }
  D2Common_3A330(h, stat[i], v);
 }
 return p5;
}
}
