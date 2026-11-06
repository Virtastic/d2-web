// D2Common 0x4A4D0: release the four grids of an outdoor block and its vertex list (ESI = outdoor data, EAX = pool;
// void). Written from the 1.13c disassembly: Fog 10046 (thunk 0x9228) is the pool FREE of each grid's row-offset block
// (ECX pool, EDX block, DrlgGrid.cpp line 0x218), not an assert; 0x1B3D0 frees the vertices (EBX pool, EDI &list).
#include <cstdint>
#include "d2common_4a4d0.h"
static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline void wr(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(uintptr_t(a)) = v; }

// 1.13c D2Common+0x4A4D0
extern "C" void D2Common_4A4D0_With(uint32_t outdoor, uint32_t pool, const D2Common4a4d0Env* e)
{
	for (uint32_t g = 0; g < 0x50; g += 0x14) {   // grids at +0, +0x14, +0x28, +0x3C: +0 cells, +4 row offsets
		const uint32_t rows = rd(outdoor + g + 4);
		if (rows) e->free(pool, rows, 0x218);
		wr(outdoor + g, 0);
		wr(outdoor + g + 4, 0);
	}
	e->freeVertices(pool, outdoor + 0x50);
}
