// 1.13c D2Common+0x3CCA0: stdcall(owner, nKey, p1, p2, holder), ret 0x14.
// A null owner is fatal 0x2D7. Otherwise it calls the 0x7C300 update with nKey, pRoom = holder+0x10
// (0 when holder is null), p1, p2, and pOwner = owner+0x48. Retail returns with the callee's EAX
// left in place; no caller reads it, so the port returns nothing. The callee comes in through env
// (the 0x7C300 port's env), so a probe can pin its sub-callees.
#include <cstdint>
#include <cstring>
#include "d2common_fatal.h"

struct D2Common7c300Env;
extern "C" void D2Common_7C300_Update(uint32_t nKey, uint32_t pRoom, uint32_t p1, uint32_t p2,
                                       uint32_t pOwner, const D2Common7c300Env* env);

namespace {
uint32_t R32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" void D2Common_3CCA0(uint32_t owner, uint32_t nKey, uint32_t p1, uint32_t p2, uint32_t holder,
                               const D2Common7c300Env* env)
{
	if (!owner) d2common_fatal(0x2d7);
	const uint32_t room = holder ? R32(holder + 0x10) : 0;
	D2Common_7C300_Update(nKey, room, p1, p2, R32(owner + 0x48), env);
}
