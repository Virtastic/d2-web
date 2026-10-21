// D2Common 1.13c batch. Unit is addressed as uint32 words (retail offsets: +0x00 type, +0x04 class,
// +0x10 mode, +0x20 seed, +0x30 speed, +0x44/+0x48 anim speed, +0x4C/+0x4E speed fields, +0x50 rec).
#include <cstdint>
#include <emscripten.h>
#include "D2Items.h"
#include "Units/Units.h"
#include "D2States.h"
#include "D2Skills.h"

void D2Common_32820_InitializeSequence(D2UnitStrc* pUnit); // real def: d2common_batch_500e0.cpp (C++ linkage)

extern "C" {
#define W __attribute__((weak))
W int32_t sub_33110(uint32_t* u);                  // 0x33110 (unmapped)
W intptr_t sub_3E980(int32_t id);                  // 0x3E980 (unmapped): row ptr
W intptr_t d2_sub_1150(int32_t id);                // 0x1150 row ptr (r[0]=id -> r[4]), hidden id operand explicit
W void d2_fn_d2common_1100(uint32_t* r);           // 0x1100: r[0]=pSeed, r[1]=nMax -> r[4]
W uint32_t Ordinal_10265(uint32_t);
W void Ordinal_10024(const void*, uint32_t);
W char DAT_6fdda728[1];
W int32_t sub_32E10(uint32_t* u);                  // 0x32E10 (unit helper)
[[noreturn]] static void fail(uint32_t c) { Ordinal_10024(DAT_6fdda728, Ordinal_10265(c)); __builtin_trap(); }

// 1.13c D2Common+0x335F0 (#10595): per-type unit speed/anim setup.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_335f0(uint32_t* u) {
 if (!u) fail(0x951);
 *(uint8_t*)((uint8_t*)u + 0x4e) = 0;
 u[0x11] = (uint32_t)UNITS_GetFrameBonus((D2UnitStrc*)u) << 8;
 int32_t type = (int32_t)u[0], cls = (int32_t)u[1], mode = (int32_t)u[4];
 D2COMMON_11013_ConvertMode((D2UnitStrc*)u, &type, &cls, &mode, nullptr, 0);
 switch (type) {
 case 0:
  if (mode == 3 || mode == 0x13) sub_32E10(u);
  if (mode != 0x12) { u[0xc] = 0; sub_33110(u); u[0x12] = (uint32_t)(*(int32_t*)(u[0x14] + 8)) << 8; return 1; }
  D2Common_32820_InitializeSequence((D2UnitStrc*)u); sub_33110(u); return 1;
 case 1:
  if (type == 0 && STATES_IsUnitShapeShifted((D2UnitStrc*)u) && (mode == 0xd || mode == 0xf)) sub_32E10(u);
  if (mode != 0xe) { sub_33110(u); u[0xc] = 0; u[0x12] = (uint32_t)(*(int32_t*)(u[0x14] + 8)) << 8; return 1; }
  D2Common_32820_InitializeSequence((D2UnitStrc*)u); sub_33110(u); return 1;
 case 2: {
  if (mode > 7) { Ordinal_10265(0x98a); Ordinal_10024(DAT_6fdda728, 0); __builtin_trap(); }
  const intptr_t r = sub_3E980(cls);
  u[0x12] = *(uint32_t*)(r + 0xd8 + mode*4);
  const int32_t s = *(int16_t*)(r + 0xf8 + mode*2);
  u[0x11] = (uint32_t)*(uint8_t*)(r + 0x129 + mode) << 8;
  if (*(char*)(r + 0x175) == 0) {
   uint32_t rg[6] = { (uint32_t)(uintptr_t)(u + 8), (uint32_t)(s >> 4), 0, 0, 0, 0 };
   d2_fn_d2common_1100(rg);
   int32_t v = (int32_t)rg[4] + (s - (s >> 4));
   *(int16_t*)(u + 0x13) = v > 0 ? (int16_t)(v > 0x7ffe ? 0x7fff : v) : 0;
   return 1;
  }
  *(int16_t*)(u + 0x13) = (int16_t)s;
  return 1; }
 case 3: {
  const intptr_t r = d2_sub_1150(cls);
  if (!r) return 0;
  const int32_t v = *(int16_t*)(r + 0xa0);
  int16_t o;
  if (v < 0x10001) o = (int16_t)((v * 0x100 + ((v * 0x100 >> 31) & 0x3ff)) >> 10);
  else if ((v & ~0xf) < 0x4000) o = (int16_t)((int64_t)v * 0x100 / 0x400);
  else o = (int16_t)(((v + ((v >> 31) & 0x3ff)) >> 10) << 8);
  *(int16_t*)(u + 0x13) = o;
  u[0x12] = (uint32_t)*(uint8_t*)(r + 0x178) << 8;
  return 1; }
 case 4:
  if (mode == 3) { *(int16_t*)(u + 0x13) = 0; u[0x12] = 0x1100; u[0x11] = 0x1000; return 1; }
  if (mode == 5) { *(int16_t*)(u + 0x13) = 0x100; u[0x12] = 0x1100; return 1; }
 }
 return 1;
}
}
