// 1.13c D2Common+0x1B1D0 (#11136): scale a monster unit's life/damage-type stats for difficulty 1 or 2 (stdcall(pUnit, nArg2, nDiff),
// ret 0xC; retail-layout seam: unit words are read at their retail 32-bit offsets, the four callees arrive through the env).
// Unit: +0 type (must be 1), +4 class id. MonStats row (via 0x12A0): +0x4C byte (1 = untouched), +0xAA int16 base level.
// Three stats per call, from the module table at RVA 0x94E3C (5 dwords per row: stat, mul1, div1, mul2, div2): each stat s becomes
// ratio(mul, div, get(s)) with the pair picked by difficulty (1 -> mul1/div1, else mul2/div2). Then stat 0xC := level + nDiff * 0x19.
#include <cstdint>

struct D2Common1B1D0Env {
  uint32_t (*monRecord)(int32_t classId);                         // 0x12A0: MonStats row pointer, 0 when out of range
  int32_t (*statGet)(uint32_t pUnit, int32_t stat);               // 0x38B70(unit, stat, 0)
  int32_t (*ratio)(int32_t mul, int32_t div, int32_t value);      // 0x11E0: eax = mul, ecx = div, edx = value, stack 0
  void (*statSet)(uint32_t pUnit, int32_t stat, int32_t value);   // 0x3A740(unit, stat, value, 0)
};

namespace {
const int32_t kRatio[3][5] = { { 7, 1, 2, 1, 2 }, { 0x1f, 0xa, 0xc, 0xa, 0xc }, { 0xd, 0xa, 0x11, 0xa, 0x1a } };
inline uint32_t W(uint32_t a) { return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(a)); }
}

extern "C" void D2Common_1B1D0_ScaleMonsterStats(uint32_t u, int32_t nArg2, uint8_t nDiff, const D2Common1B1D0Env* env) {
  if (nArg2 != 0 || nDiff == 0 || u == 0 || W(u) != 1) return;
  const uint32_t rec = env->monRecord(static_cast<int32_t>(W(u + 4)));
  if (!rec) return;
  if (*reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(rec) + 0x4C) == 1) return;
  for (int i = 0; i < 3; ++i) {
    const int32_t* row = kRatio[i];
    const int32_t v = env->statGet(u, row[0]);
    const int32_t nv = nDiff == 1 ? env->ratio(row[1], row[2], v) : env->ratio(row[3], row[4], v);
    env->statSet(u, row[0], nv);
  }
  env->statSet(u, 0xC, *reinterpret_cast<const int16_t*>(static_cast<uintptr_t>(rec) + 0xAA) + nDiff * 0x19);
}
