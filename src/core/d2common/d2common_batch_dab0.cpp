// D2Common implementation batch: one UNMAPPED row translated from the pinned 1.13c decompile.
#include <cstdint>

#include <emscripten.h>

#include "Fog.h"
#include "Path/Path.h"
#include "Path/Step.h"
#include "Units/Units.h"

// 1.13c D2Common+0x0DAB0 (#11121): D2Common_10235 / PATH_UpdateRiderPath -- sync a rider's dynamic
// path to its mount's. Flag 0x8 is cleared when both paths hold the same tile position and set
// otherwise; flag 0x1 is set when the two paths sit in different rooms. The mount's precise
// coordinates and room are then pushed through the 1.13c room-recache helper at 0xDA40, which
// refuses the move (and ends a missile's path) when the destination is outside the rider's room
// list. A null rider or mount is fatal (0x54E / 0x54F) in retail.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_dab0(D2UnitStrc* pRiderUnit, D2UnitStrc* pMountUnit)
{
	D2_ASSERT(pRiderUnit);
	D2_ASSERT(pMountUnit);

	D2DynamicPathStrc* pRiderPath = pRiderUnit->pDynamicPath;
	D2DynamicPathStrc* pMountPath = pMountUnit->pDynamicPath;

	if (pRiderPath->tGameCoords.wPosX == pMountPath->tGameCoords.wPosX
		&& pRiderPath->tGameCoords.wPosY == pMountPath->tGameCoords.wPosY)
	{
		pRiderPath->dwFlags &= ~PATH_UNKNOWN_FLAG_0x00008;
	}
	else
	{
		pRiderPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00008;
	}

	if (pRiderPath->pRoom != pMountPath->pRoom)
	{
		pRiderPath->dwFlags |= PATH_UNKNOWN_FLAG_0x00001;
	}

	PATH_RecacheRoomAtCoordIfNeeded(pRiderPath, pMountPath->pRoom,
		pMountPath->tGameCoords.dwPrecisionX, pMountPath->tGameCoords.dwPrecisionY);
}
