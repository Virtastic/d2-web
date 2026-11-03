// D2Common 1.13c batch. 0x335F0 uses the engine's D2UnitStrc by field name (wasm64 widened pointers; the
// x86 offsets are +0x00 type, +0x04 class, +0x10 mode, +0x20 seed, +0x30 pAnimSeq, +0x44/+0x48 frame fields,
// +0x4C/+0x4E speed fields, +0x50 pAnimData).
#include <cstdint>
#include <emscripten.h>
#include "D2Items.h"
#include "Units/Units.h"
#include "D2States.h"
#include "D2Skills.h"

void D2Common_32820_InitializeSequence(D2UnitStrc* pUnit); // real def: d2common_batch_500e0.cpp (C++ linkage)
void __stdcall UNITS_CharacterStartRunningOrKnockback(D2UnitStrc* pUnit, int nClassId); // 0x32E10 (Units.cpp): retail EAX = converted class

extern "C" {
#define W __attribute__((weak))
W intptr_t sub_3E980(int32_t id);                  // 0x3E980 (unmapped): row ptr
W intptr_t d2_sub_1150(int32_t id);                // 0x1150 row ptr (r[0]=id -> r[4]), hidden id operand explicit
W void d2_fn_d2common_1100(uint32_t* r);           // 0x1100: r[0]=pSeed, r[1]=nMax -> r[4]
W uint32_t Ordinal_10265(uint32_t);
W void Ordinal_10024(const void*, uint32_t);
W char DAT_6fdda728[1];
[[noreturn]] static void fail(uint32_t c) { Ordinal_10024(DAT_6fdda728, Ordinal_10265(c)); __builtin_trap(); }

// 1.13c D2Common+0x335F0 (#10595): per-type unit speed/anim setup.
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_335F0_UnitSetupSpeeds(D2UnitStrc* u) {
 if (!u) fail(0x951);
 u->nActionFrame = 0;
 u->nSeqCurrentFramePrecise = UNITS_GetFrameBonus(u) << 8;
 int32_t type = (int32_t)u->dwUnitType, cls = (int32_t)u->dwClassId, mode = (int32_t)u->dwAnimMode;
 D2COMMON_11013_ConvertMode(u, &type, &cls, &mode, nullptr, 0);
 switch (type) {
 case 0:
  if (mode == 3 || mode == 0x13) UNITS_CharacterStartRunningOrKnockback(u, cls);
  if (mode != 0x12) { u->pAnimSeq = nullptr; UNITS_UpdateAnimRateAndVelocity(u, nullptr, 0); u->dwFrameCountPrecise = (int32_t)u->pAnimData->dwFrames << 8; return 1; }
  D2Common_32820_InitializeSequence(u); UNITS_UpdateAnimRateAndVelocity(u, nullptr, 0); return 1;
 case 1:
  if (u->dwUnitType == 0 && STATES_IsUnitShapeShifted(u) && (mode == 0xd || mode == 0xf)) UNITS_CharacterStartRunningOrKnockback(u, cls);
  if (mode != 0xe) { UNITS_UpdateAnimRateAndVelocity(u, nullptr, 0); u->pAnimSeq = nullptr; u->dwFrameCountPrecise = (int32_t)u->pAnimData->dwFrames << 8; return 1; }
  D2Common_32820_InitializeSequence(u); UNITS_UpdateAnimRateAndVelocity(u, nullptr, 0); return 1;
 case 2: {
  if (mode > 7) { Ordinal_10265(0x98a); Ordinal_10024(DAT_6fdda728, 0); __builtin_trap(); }
  const intptr_t r = sub_3E980(cls);
  u->dwFrameCountPrecise = *(int32_t*)(r + 0xd8 + mode*4);
  const int32_t s = *(int16_t*)(r + 0xf8 + mode*2);
  u->nSeqCurrentFramePrecise = (int32_t)*(uint8_t*)(r + 0x129 + mode) << 8;
  if (*(char*)(r + 0x175) == 0) {
   uint32_t rg[6] = { (uint32_t)(uintptr_t)&u->pSeed, (uint32_t)(s >> 4), 0, 0, 0, 0 };
   d2_fn_d2common_1100(rg);
   int32_t v = (int32_t)rg[4] + (s - (s >> 4));
   u->wAnimSpeed = v > 0 ? (int16_t)(v > 0x7ffe ? 0x7fff : v) : 0;
   return 1;
  }
  u->wAnimSpeed = (int16_t)s;
  return 1; }
 case 3: {
  const intptr_t r = d2_sub_1150(cls);
  if (!r) return 0;
  const int32_t v = *(int16_t*)(r + 0xa0);
  int16_t o;
  if (v < 0x10001) o = (int16_t)((v * 0x100 + ((v * 0x100 >> 31) & 0x3ff)) >> 10);
  else if ((v & ~0xf) < 0x4000) o = (int16_t)((int64_t)v * 0x100 / 0x400);
  else o = (int16_t)(((v + ((v >> 31) & 0x3ff)) >> 10) << 8);
  u->wAnimSpeed = o;
  u->dwFrameCountPrecise = (int32_t)*(uint8_t*)(r + 0x178) << 8;
  return 1; }
 case 4:
  if (mode == 3) { u->wAnimSpeed = 0; u->dwFrameCountPrecise = 0x1100; u->nSeqCurrentFramePrecise = 0x1000; return 1; }
  if (mode == 5) { u->wAnimSpeed = 0x100; u->dwFrameCountPrecise = 0x1100; return 1; }
 }
 return 1;
}
}
