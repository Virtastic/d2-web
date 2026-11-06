// D2Common 0x4A9B0: outdoor room tile setup (ESI = room, void). Written from the 1.13c decompile and the retail
// instruction order; every callee goes through the Env seams so the FN probe can log retail's calls.
#include <cstdint>
#include "d2common_4a9b0.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline void wr(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(uintptr_t(a)) = v; }

// 1.13c D2Common+0x4A9B0
extern "C" void D2Common_4A9B0_With(uint32_t room, const D2Common4a9b0Env* e)
{
	// Tile grid (room+0x54): allocated from the drlg pool (room+0x58 level, +0x1B4 drlg, +0x478 pool) and zeroed.
	if (rd(room + 0x54) == 0) {
		const uint32_t pool = rd(rd(rd(room + 0x58) + 0x1B4) + 0x478);
		const uint32_t g = e->alloc(pool, 0x2C, 0x533);
		wr(room + 0x54, g);
		for (uint32_t i = 0; i < 0x2C; i += 4) wr(g + i, 0);
	}
	const uint32_t outd = rd(room + 0x20);   // +0 tile-type grid, +0x14 wall grid, +0x28 floor grid
	const uint32_t wall = outd + 0x14, floor = outd + 0x28;
	e->countWarp(room, wall, outd, 0, 0);    // 0x685D0: EAX room, EBX wall, EDI type, EDX 0, stack 0
	const uint32_t a1 = e->countTypes(room, wall);   // 0x68660: EAX room, stack grid, 0, 0, 0
	const uint32_t a2 = e->countTypes(a1, floor);    // EAX is not reloaded: the first call's return
	e->allocData(a2);                                // 0x68040: EAX = the second call's return
	e->loadTiles(room, wall, outd);          // 0x69B50: EDI room, stack grid, type, 0, 0, 0
	e->loadTiles(room, floor, 0);
	const uint32_t g = rd(room + 0x54);
	wr(g + 0x18, rd(g + 0x08));
	wr(g + 0x20, rd(g + 0x0C));
	wr(g + 0x28, rd(g + 0x10));
	e->coordLists(room);                     // 0x81100: EBX room
}
