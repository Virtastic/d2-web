// D2Common 1.13c batch. Unit is addressed as uint32 words (+0x00 type, +0x04 class, +0x10 mode,
// +0x2C base-speed scratch, +0x4C short speed, +0x50 row ptr, +0x60 stat list ptr).
#include <cstdint>
#include <emscripten.h>

extern "C" {
#define W __attribute__((weak))
W uint8_t* sgptDataTables;
W char DAT_6fded378[1];
W void sub_501E0(uint32_t* u, uint32_t* t, uint32_t* c, uint32_t* m);
W void sub_32670(uint32_t* u, uint32_t t, uint32_t c, uint32_t m);
W int32_t sub_30B80(uint32_t mode, uint32_t* u);
W int32_t sub_323E0(uint32_t* u);
W int32_t sub_2EF90(uint32_t mode);
W int32_t sub_2EFC0(uint32_t mode);
W int32_t sub_2F010(uint32_t mode);
W int32_t sub_2FB10(uint32_t type, uint32_t mode);
W void sub_34D40(uint32_t path, int32_t v);
W int32_t sub_30C70(uint32_t mode, uint32_t type, uint32_t* u);
W int32_t sub_38B70(intptr_t u, int32_t stat);
W int32_t sub_30D50(uint32_t type);
W int32_t sub_30BD0(uint32_t mode, uint32_t type, uint32_t* u, uint32_t cls);
W void sub_32EF0(uint32_t* u, int32_t f);
W int32_t sub_30530(uint32_t* u);
W intptr_t sub_1E1B0(const char* env);
W int32_t sub_23EF0(intptr_t u);
W uint32_t sub_24430(intptr_t u, int32_t t);
W int32_t sub_33C20(uint32_t tbl);
W int32_t sub_50CE0(uint32_t* u);
W uint32_t sub_37DB0(int32_t bit);
W int32_t sub_2FAC0(uint32_t mode, uint32_t cls);

static void st(uint32_t* u, uint32_t v) { // clamp-store the short at +0x4C (tail shared by branches)
 *(int16_t*)(u + 0x13) = (int32_t)v >= 0x7fff ? (int16_t)0x7fff : (int16_t)v;
}
static intptr_t weapon(uint32_t* pl, uint32_t off) {
 intptr_t r = 0;
 if (pl[0x18] && *(uint32_t*)(intptr_t)pl[0x18] == 0x1020304) {
  intptr_t e = sub_1E1B0(DAT_6fded378);
  if (e) r = (intptr_t)*(uint32_t*)(*(uint32_t*)(e + 0xc) + off);
 }
 if (!sub_23EF0(r)) r = 0;
 return r;
}

// 1.13c D2Common+0x33110: derive the unit's base speed short (+0x4C) from mode and row (+0x50).
EMSCRIPTEN_KEEPALIVE int32_t sub_33110(uint32_t* u) {
 if (!u) return 0;
 uint32_t type = u[0];
 if ((int32_t)type >= 4) return 0;
 uint32_t mode = u[4], cls = u[1];
 sub_501E0(u, &type, &cls, &mode);
 const uint32_t t = type, m = mode;
 sub_32670(u, t, cls, m);
 const intptr_t row = (intptr_t)u[0x14];
 const int32_t rowv = *(int32_t*)(row + 0xc);
 if (sub_30B80(m, u)) {
  int32_t p = sub_323E0(u) + 100;
  if (p > 0xaf) p = 0xaf;
  uint32_t v = (uint32_t)(rowv * p) / 100;
  if (v) { if (v > 0x7ffe) v = 0x7fff; *(int16_t*)(u + 0x13) = (int16_t)v; u[0xf] = (uint32_t)(int32_t)(int16_t)v; }
  else { *(int16_t*)(u + 0x13) = 0; u[0xf] = 0; }
  return 0;
 }
 uint32_t v;
 if (sub_2EF90(m)) {
  int32_t p = sub_323E0(u), b = 0x32;
  if ((int32_t)u[0] >= 0 && ((int32_t)u[0] < 2 || u[0] == 3) && *(int32_t*)(sgptDataTables + 0xc4) > 0x65 && sub_37DB0(0x65)) b = 100;
  v = (uint32_t)((b + p) * rowv) / 100;
  if (v == 0) v = 1; // retail stores 1 here
  st(u, v);
  return 0;
 }
 if (sub_2EFC0(m)) {
  v = (uint32_t)((sub_323E0(u) + 0x32) * rowv) / 100;
  if (v == 0) { *(int16_t*)(u + 0x13) = 0; return 0; }
  st(u, v); return 0;
 }
 if (sub_2F010(m)) {
  int32_t s = sub_2FB10(t, m);
  s = s < 1 ? 0 : s > 0x7ffe ? 0x7fff : s;
  *(int16_t*)(u + 0x13) = (int16_t)s;
  sub_34D40(u[0xb], 0x1000);
  return 0;
 }
 if (sub_30C70(m, t, u)) {
  uint32_t path = u[0xb];
  if (!path) return 0;
  int32_t p = sub_38B70((intptr_t)u, 0x43) + sub_323E0(u);
  if (p < 0x19) p = 0x19;
  int32_t s = (sub_2FB10(t, m) * p) / 100;
  s = s < 1 ? 0 : s > 0x7ffe ? 0x7fff : s;
  *(int16_t*)(u + 0x13) = (int16_t)s;
  sub_34D40(path, (sub_30D50(t) * p) / 100);
  return 0;
 }
 if (sub_30BD0(m, t, u, cls)) {
  sub_32EF0(u, 1);
  int32_t p = sub_38B70((intptr_t)u, 0x44) + sub_323E0(u);
  if (sub_30530(u)) {
   intptr_t a = weapon(u, 0x10), b = weapon(u, 0x14);
   if (sub_24430(a, 0x2d) && sub_24430(b, 0x2d)) {
    int32_t x = sub_38B70(a, 0x44), y = sub_38B70(b, 0x44);
    p += (y + x) / 2 - x;
   }
  }
  if (u[0] == 0 && u[4] == 0x12) p -= 0x1e;
  p = p < 0xf ? 0xf : p > 0xaf ? 0xaf : p;
  int32_t base;
  base = rowv;
  if (sub_33C20(*(uint32_t*)(sgptDataTables + 0x164))) { int32_t w = sub_50CE0(u); if (w) base = w; }
  v = (uint32_t)(base * p) / 100;
  if (v) { if (v > 0x7ffe) v = 0x7fff; u[0xf] = (uint32_t)(int32_t)(int16_t)v; *(int16_t*)(u + 0x13) = (int16_t)v; }
  else { u[0xf] = 0; *(int16_t*)(u + 0x13) = 0; }
  return 0;
 }
 if (sub_2FAC0(m, cls)) {
  if (rowv < 1) { *(int16_t*)(u + 0x13) = 0; return 0; }
  st(u, (uint32_t)rowv); return 0;
 }
 int32_t p = sub_38B70((intptr_t)u, 0x45);
 p = p < 0xf ? 0xf : p > 0xaf ? 0xaf : p;
 v = (uint32_t)(rowv * p) / 100;
 if (v == 0) { *(int16_t*)(u + 0x13) = 0; return 0; }
 st(u, v);
 return 0;
}
}
