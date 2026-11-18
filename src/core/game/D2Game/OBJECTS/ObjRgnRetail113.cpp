#include <test_seam.h>
#include "OBJECTS/ObjRgnRetail113.h"
#include "ITEMS/ItemsRetail113.h"

#include <cstring>

// Retail-layout (1.13c) object-region control teardown. Addresses are the 32-bit retail ones; Fog's free traps here (the game
// has no retail-layout caller) and the proofs replace it through D2Seam. Written from the 1.13c disassembly.

uint32_t D2Retail113_FogFree_A2EC(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) { __builtin_trap(); }

namespace
{
inline uint32_t Rd32(uint32_t a, uint32_t o = 0) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
inline void Wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)a + o, &v, 4); }
}

// 1.13c D2Game+0xB8A0: a null game is a fatal error in retail (not modelled). The object control game+0x10F0, if any: each node of its
// +0x1110 chain (next at +0xC, read before the free) is freed (line 0xC1), the chain head cleared; then for i = 1 .. dt+0xC5C - 1 (the
// bound reread each pass) the slot control+0x4C + 4 * (i - 1) is freed whether null or not (line 0xE2); the eight slots at +0x28 are
// freed when nonzero (line 0xE8); last the control itself, reread from game+0x10F0 (line 0xEC).
uint32_t D2Retail113_B8A0(uint32_t game)
{
	const uint32_t F = 0xB20FD174u;   // the objrgn.cpp source-name word the frees carry
	if (!game) return 0;
	const uint32_t c = Rd32(game, 0x10F0);
	if (!c) return 0;
	const uint32_t dt = D2_CALL(D2Retail113_DataTables);
	for (uint32_t n = Rd32(c, 0x1110); n;)
	{
		const uint32_t nx = Rd32(n, 0xC);
		D2_CALL(D2Retail113_FogFree_A2EC, Rd32(game, 0x1C), n, F, 0xC1, 0);
		n = nx;
	}
	Wr32(c, 0x1110, 0);
	for (int32_t i = 1; i < (int32_t)Rd32(dt, 0xC5C); i++)
		D2_CALL(D2Retail113_FogFree_A2EC, Rd32(game, 0x1C), Rd32(c, 0x4C + 4 * (uint32_t)(i - 1)), F, 0xE2, 0);
	for (uint32_t i = 0; i < 8; i++)
		if (Rd32(c, 0x28 + 4 * i)) D2_CALL(D2Retail113_FogFree_A2EC, Rd32(game, 0x1C), Rd32(c, 0x28 + 4 * i), F, 0xE8, 0);
	return D2_CALL(D2Retail113_FogFree_A2EC, Rd32(game, 0x1C), Rd32(game, 0x10F0), F, 0xEC, 0);
}
