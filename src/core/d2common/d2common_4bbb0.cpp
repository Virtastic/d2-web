// The small-unit presence reset: nMask is cleared on the plus-shaped five-subtile footprint centred on
// (nX, nY). 1.13c factors this out of COLLISION_ResetMaskWithPattern's first case; each cell resolves its
// own room through the neighbour search, so a footprint straddling a room edge clears both rooms.
// Retail: cdecl (pRoom, nX, nMask), nY in ESI.
#include <cstdint>

#include "D2Collision.h"

// 1.13c D2Common+0x4BBB0
void COLLISION_ResetSmallUnitPresence(D2ActiveRoomStrc* pRoom, int nX, int nY, uint16_t nMask)
{
	COLLISION_ResetCollisionMask(pRoom, nX - 1, nY, nMask);
	COLLISION_ResetCollisionMask(pRoom, nX, nY, nMask);
	COLLISION_ResetCollisionMask(pRoom, nX + 1, nY, nMask);
	COLLISION_ResetCollisionMask(pRoom, nX, nY - 1, nMask);
	COLLISION_ResetCollisionMask(pRoom, nX, nY + 1, nMask);
}
