// 1.13c D2Common+0x09C60 (sub_9C60): usercall, the map (D2DrlgMapStrc) arrives in ESI, count in the stack arg.
// Sets nPops = n, then for each pops rect (pPopsLocation, 4 ints x,y,w,h holding two corners as x0,y0,x1,y1):
// normalises to min/max, rebases onto the map origin (pDrlgCoord +0x10/+0x14), stores size as max-min+1,
// and turns pPopsIndex[i] into (v/4)-1 (round toward zero). Sole caller DRLGPRESET_BuildPresetArea (0x0C060).
#include <cstdint>
#include "Drlg/D2DrlgPreset.h"

void D2Common_9C60_SetPops(D2DrlgMapStrc* pMap, int32_t nCount)
{
	pMap->nPops = nCount;
	for (int32_t i = 0; i < nCount; ++i)
	{
		int32_t* p = (int32_t*)&pMap->pPopsLocation[i];
		const int32_t x0 = p[0], y0 = p[1], x1 = p[2], y1 = p[3];
		const int32_t minX = x0 < x1 ? x0 : x1, maxX = x0 > x1 ? x0 : x1;
		const int32_t minY = y0 < y1 ? y0 : y1, maxY = y0 > y1 ? y0 : y1;
		p[0] = pMap->pDrlgCoord.nPosX + minX;
		p[1] = pMap->pDrlgCoord.nPosY + minY;
		p[2] = maxX - minX + 1;
		p[3] = maxY - minY + 1;
		const int32_t v = pMap->pPopsIndex[i];
		pMap->pPopsIndex[i] = v / 4 - 1;   // signed divide, truncating (the decompile's (v + ((v>>31)&3)) >> 2 is that divide)
	}
}
