// 1.13c D2Common+0x3CBE0: stdcall(owner, nKey, p1, p2, holder), ret 0x14.
// A null owner is fatal 0x2E9. Otherwise it calls the 0x7BF20 list update with pOwner = owner+0x48,
// pRoom = holder+0x10 (0 when holder is null), and the key and the two words as given. Retail
// returns with whatever EAX the callee left, and no caller reads it, so the port returns nothing.
// The callee is passed in through env (the 0x7BF20 port's env), so a probe can pin its sub-callees.
#include <cstdint>
#include <cstring>
#include "d2common_fatal.h"

struct D2Common7bf20Env;
extern "C" void D2Common_7BF20_Update(uint32_t pOwner, uint32_t pRoom, uint32_t nKey, uint32_t p1,
                                       uint32_t p2, const D2Common7bf20Env* env);

namespace {
uint32_t R32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" void D2Common_3CBE0(uint32_t owner, uint32_t nKey, uint32_t p1, uint32_t p2, uint32_t holder,
                               const D2Common7bf20Env* env)
{
	if (!owner) d2common_fatal(0x2e9);
	const uint32_t room = holder ? R32(holder + 0x10) : 0;
	D2Common_7BF20_Update(R32(owner + 0x48), room, nKey, p1, p2, env);
}
