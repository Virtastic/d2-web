// 1.13c D2Common+0x1A080: run a unit's three one-shot init hooks, then return its +0x30 word (retail-layout seam;
// the three callees are still-unported retail routines and arrive through the env). EAX = pUnit.
// Unit: +0x28 done-flags (0x1000000, 0x2000000, 0x100000), +0x30 result, +0x48 kind, +0x58 -> level
// (+0x1b4 -> +0x478 is handed to the third hook).
#include <cstdint>

struct D2CommonUnitInitEnv {
  void (*f68400)(uint32_t pUnit);              // 0x68400, ESI = unit
  void (*fBF70)(uint32_t pUnit);               // 0xBF70, stdcall(unit)
  void (*f7C0E0)(uint32_t pUnit, uint32_t ctx);// 0x7C0E0, EAX = unit, ECX = ctx
};

namespace {
inline uint32_t W(uint32_t a) { return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(a)); }
}

extern "C" uint32_t D2Common_1A080_UnitInit(uint32_t u, const D2CommonUnitInitEnv* env) {
  if (!(W(u + 0x28) & 0x1000000)) env->f68400(u);
  if (!(W(u + 0x28) & 0x2000000) && W(u + 0x48) == 2) env->fBF70(u);
  if (!(W(u + 0x28) & 0x100000)) env->f7C0E0(u, W(W(W(u + 0x58) + 0x1b4) + 0x478));
  return W(u + 0x30);
}
