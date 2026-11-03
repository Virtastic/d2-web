// 1.13c D2Common+0x3CEE0: stdcall(unit, nId, nMode, pY, pX), ret 0x14.
// A null unit is fatal 0x221. Otherwise it calls the 0x1A130 level-anchor resolver with the
// retail register mapping: EAX = pX (the fifth argument, which the first batch port dropped),
// ECX = unit+0x48 (the drlg), EDX = nId, stack = nMode, pY. The resolver's result stays in EAX
// and is returned. The resolver comes in through env (the 0x1A130 port's env).
#include <cstdint>
#include <cstring>
#include "d2common_fatal.h"

struct D2Common1A130Env;
extern "C" uint32_t D2Common_1A130_LevelAnchorRaw(const D2Common1A130Env* e, uint32_t pX, uint32_t drlg,
                                                  int32_t id, int32_t mode, uint32_t pY);

namespace {
uint32_t R32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" uint32_t D2Common_3CEE0(uint32_t unit, int32_t nId, int32_t nMode, uint32_t pY, uint32_t pX,
                                   const D2Common1A130Env* env)
{
	if (!unit) d2common_fatal(0x221);
	return D2Common_1A130_LevelAnchorRaw(env, pX, R32(unit + 0x48), nId, nMode, pY);
}
