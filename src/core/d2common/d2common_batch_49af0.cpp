// D2Common 1.13c batch. Unwired retail callees are weak so the file links; sgptDataTables and
// DAT_6fdeff6c are weak globals.
#include <cstdint>
#include <emscripten.h>

extern "C" {
#define W __attribute__((weak))
W int32_t sub_464D0(int32_t a, const int32_t* item);                       // 0x464D0
W void sub_47EF0(int32_t, void*, int32_t, int32_t, int32_t, uint32_t);     // 0x47EF0
W uint32_t* sub_47020(void*, int32_t, uint32_t);                           // 0x47020
W void sub_3A330(uint32_t*, int32_t, int32_t);                             // 0x3A330
W int32_t sub_38B00(intptr_t);                                             // 0x38B00
W intptr_t DATATBLS_GetItemsTxtRecord(int32_t);
W uint32_t Ordinal_10265(uint32_t);
W void Ordinal_10024(const void*, uint32_t);
W void Ordinal_10213(intptr_t, int32_t, int32_t);
W uint8_t* sgptDataTables;
W intptr_t DAT_6fdeff6c;
extern "C" __attribute__((weak)) const char DAT_6fdda728[1];

[[noreturn]] static void fail(uint32_t c) { Ordinal_10024(DAT_6fdda728, Ordinal_10265(c)); __builtin_trap(); }

static int32_t chk(int32_t p1, const int32_t* it, int32_t p4) {
 int32_t r = sub_464D0(*(int32_t*)(p4 + 8), it);
 if (p1 == 1) {
  if (it[0] != 4) fail(0x61c);
  intptr_t rec = DATATBLS_GetItemsTxtRecord(it[1]);
  if (!rec) fail(0x61f);
  Ordinal_10213(DAT_6fdeff6c, *(int32_t*)(rec + 0x80), 0);
 }
 return r;
}

// 1.13c D2Common+0x49AF0: apply the four per-slot stat adjustments (0x28/0x2a/0x2c/0x2e) for an item.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_49af0(int32_t p1, void* p2, int32_t* p3, int32_t p4, uint32_t p5, int32_t p6, int32_t p7, uint32_t p8) {
 if (!p3) fail(0x1505);
 if (*p3 != 4) fail(0x1506);
 if (!p3[5]) fail(0x1508);
 if (*(int16_t*)((intptr_t)p3[5] + 0x30) != 0) {
  if (!p4) fail(0x364);
  int32_t r = sub_464D0(*(int32_t*)(p4 + 8), p3);
  for (int i = 0; i < 4; ++i) sub_47EF0(p1, p2, r, p6, p7, p8);
  return p5;
 }
 static const struct { int32_t stat, cnt, off; } S[4] = {{0x28,0x29,0x32a0},{0x2a,0x2b,0x3528},{0x2c,0x2d,0x37b0},{0x2e,0x2f,0x3a38}};
 int32_t r = chk(p1, p3, p4);
 for (int i = 0; i < 4; ++i) {
  if (i) r = chk(p1, p3, p4);
  if (!r) { if (i == 3) return p5; continue; }
  uint32_t* u = sub_47020(p2, p7, p8);
  if (!u) fail(0x29d);
  if (p6) {
   intptr_t base = *(intptr_t*)(sgptDataTables + 0xbcc) + S[i].off;
   r = (*(int32_t*)(sgptDataTables + 0xbd4) < S[i].cnt || !base) ? 0 : sub_38B00(base);
   r = -r;
  }
  sub_3A330(u, S[i].stat, r);
 }
 return p5;
}
}
