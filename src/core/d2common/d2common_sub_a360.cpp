// 1.13c D2Common+0x0A360 (sub_A360, usercall: context in EBX, no stack args; sole caller +0xAB50).
// Walks the singly linked preset-unit list of the room's map (room +0x20 -> map pointer +8 -> list head +0x38 in
// retail). A unit whose (x, y) lies inside the rect [dx*5, dx*5+w*5) x [dy*5, dy*5+h*5) is shifted by
// (-dx*5, -dy*5), unlinked, and pushed on the caller's free list (context +0x5c). Others stay in place.
// The context's rect lives at +0x34 (dx), +0x38 (dy), +0x3c (w), +0x40 (h). Our engine is m64, so the retail
// 4-byte offsets are documentation: the engine's own preset structs are used, and the probe marshals the blob.
#include <cstdint>

#include "Drlg/D2DrlgPreset.h"

void D2Common_A360_MoveNodesInRect(D2DrlgPresetRoomStrc* pMaze, int32_t dx, int32_t dy, int32_t w, int32_t h, D2PresetUnitStrc** ppFreeList)
{
	D2DrlgMapStrc* pMap = pMaze->pMap;
	const int32_t x0 = dx * 5, y0 = dy * 5;
	D2PresetUnitStrc* pPrev = nullptr;
	for (D2PresetUnitStrc* pUnit = pMap->pPresetUnit; pUnit;)
	{
		D2PresetUnitStrc* pNext = pUnit->pNext;
		if (pUnit->nXpos < x0 || pUnit->nYpos < y0 || pUnit->nXpos >= w * 5 + x0 || pUnit->nYpos >= y0 + h * 5)
		{
			pPrev = pUnit;
		}
		else
		{
			pUnit->nXpos -= x0;
			pUnit->nYpos -= y0;
			(pPrev ? pPrev->pNext : pMap->pPresetUnit) = pNext;
			pUnit->pNext = *ppFreeList;
			*ppFreeList = pUnit;
		}
		pUnit = pNext;
	}
}
