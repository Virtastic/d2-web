// 1.13c D2Common+0x314A0 (#10839): UNITS_TestCollisionWithUnit on two units (stdcall, three
// arguments, ret 0xC). The room comes from the first unit (UNITS_GetRoom); both coordinate pairs
// and both X sizes feed 0x30730 (UNITS_TestCollision) with the collision mask. Retail fatals
// 0x1290 / 0x1291 on a null first / second unit; the port aborts the same way. A unit with no
// room answers 0.
#include <cstdint>
#include <cstdlib>
#include "D2Collision.h"
#include "Units/Units.h"

extern "C" int32_t D2Common_314A0_TestCollisionWithUnit(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2, uint32_t nMask) // 1.13c D2Common+0x314A0
{
    if (!pUnit1 || !pUnit2)
        std::abort();

    D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit1);
    if (!pRoom)
        return 0;

    D2CoordStrc coords1 = {};
    D2CoordStrc coords2 = {};
    UNITS_GetCoords(pUnit1, &coords1);
    UNITS_GetCoords(pUnit2, &coords2);

    return UNITS_TestCollision(coords1.nX, coords1.nY, UNITS_GetUnitSizeX(pUnit1),
                               coords2.nX, coords2.nY, UNITS_GetUnitSizeX(pUnit2),
                               pRoom, static_cast<uint16_t>(nMask));
}
