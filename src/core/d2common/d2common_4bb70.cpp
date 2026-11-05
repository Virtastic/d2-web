// 1.13c D2Common+0x4BB70: clear the low 16 bits of nMask (ECX) in one subtile of the room's collision grid.
// EAX = room, EDX = y, stack word = x (ret 4). The record 0x1330 returns holds the grid at +0x20; the grid
// is {x0 +0, y0 +4, width +8, ..., cells +0x20}, cells 16-bit, index (y - y0) * width - x0 + x. A missing
// record, grid or cell array leaves everything alone. Retail does the index and address in 32 bits.
#include <cstring>
#include "d2common_4bb70.h"

namespace
{
	uint32_t Rd(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

void D2Common_4BB70_ClearSubtileMask(uint32_t pRoom, uint32_t nMask, int32_t nY, int32_t nX, D2RoomCollisionFn pfnCollision)
{
	const uint32_t pRecord = pfnCollision(pRoom, nX, nY);
	if (!pRecord) return;
	const uint32_t pGrid = Rd(pRecord + 0x20);
	if (!pGrid) return;
	const uint32_t pCells = Rd(pGrid + 0x20);
	if (!pCells) return;
	const uint32_t nIndex = ((uint32_t)nY - Rd(pGrid + 4)) * Rd(pGrid + 8) - Rd(pGrid) + (uint32_t)nX;
	uint16_t* pCell = reinterpret_cast<uint16_t*>(static_cast<uintptr_t>(pCells + nIndex * 2));
	*pCell &= (uint16_t)~nMask;
}
