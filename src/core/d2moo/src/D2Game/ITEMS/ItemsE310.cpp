#include "ITEMS/Items.h"

#include <test_seam.h>
#include <D2Dungeon.h>
#include <Drlg/D2DrlgDrlg.h>
#include <Units/Units.h>

#include "GAME/Game.h"

// Written from the 1.13c decompile: no D2MOO 1.10f counterpart. The unit's room gives the level and so the act; a unit
// in a non-normal game moves two rows on. Past the table's end the fourth entry answers.
// A unit with no room is fatal in retail (log, exit); the game asserts here.
// 1.13c D2Game+0xE310
int32_t __fastcall ITEMS_GetDropCodeByActAndDifficulty(D2UnitStrc* pUnit, const int32_t* pCodes, int32_t nCount)
{
    D2ActiveRoomStrc* pRoom = D2_CALL(UNITS_GetRoom, pUnit);
    D2_ASSERT(pRoom);

    uint8_t nIndex = D2_CALL(DRLG_GetActNoFromLevelId, D2_CALL(DUNGEON_GetLevelIdFromRoom, pRoom));
    if (pUnit->pGame && pUnit->pGame->nDifficulty != 0)
    {
        nIndex += 2;
    }

    if (nCount <= nIndex)
    {
        return pCodes[3];
    }
    return pCodes[nIndex];
}
