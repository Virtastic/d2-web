// 1.13c D2Common+0x31850 (#10032): set the collision mask of a unit's footprint in a room (stdcall,
// four arguments, ret 0x10). The unit size X (0x30F80) and Y (0x30EE0) come from the unit; when both
// are nonzero the mask (0x2FF30) is written over the box with COLLISION_SetMaskWithSizeXY (0x4B840).
// A null unit is retail's fatal 0x851 (_exit), so the port aborts the same way.
#include <cstdint>
#include <cstdlib>
#include "D2Collision.h"
#include "Units/Units.h"

extern "C" void D2Common_31850_SetCollisionWithSize(D2UnitStrc* pUnit, D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY) // 1.13c D2Common+0x31850
{
	if (!pUnit)
		std::abort();

	const int nSizeX = UNITS_GetUnitSizeX(pUnit);
	const int nSizeY = UNITS_GetUnitSizeY(pUnit);

	if (nSizeX != 0 && nSizeY != 0)
	{
		const int nMask = UNITS_GetCollisionMask(pUnit);
		COLLISION_SetMaskWithSizeXY(pRoom, nX, nY, (unsigned int)nSizeX, (unsigned int)nSizeY, (uint16_t)nMask);
	}
}
