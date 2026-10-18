// 1.13c D2Common implementation batch (rows 0x31940, 0x3C390, 0x228D0, 0x330F0, 0x30670, 0x242D0).
// Each body delegates to the vendored D2MOO 1.10f function whose 1.10f/1.13c behaviour was matched
// line by line against the decompile and disassembly; 0x31940 has no 1.10f twin and is written here.
#include <cstdint>
#include "D2DataTbls.h"
#include "D2Dungeon.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "Units/Units.h"

// 1.13c D2Common+0x31940: the unit's "level" stat total (ItemStatCost row 12, packed id 0xC0000)
// scaled by 10000. Guards: null unit, null statlist, fewer than 13 ItemStatCost rows, null row.
// 1.10f has no equivalent helper; the body follows the decompile stores directly.
uint32_t D2Common_10049_GetUnitLevelStatScaled(D2UnitStrc* pUnit)
{
    if (!pUnit || !pUnit->pStatListEx)
    {
        return 0;
    }
    if (sgptDataTables->nItemStatCostTxtRecordCount <= 0xc || !sgptDataTables->pItemStatCostTxt)
    {
        return 0;
    }
    const int nValue = STATLIST_GetTotalStat_6FDB63E0(
        STATLIST_StatListExCast(pUnit->pStatListEx), 0xC0000, &sgptDataTables->pItemStatCostTxt[12]);
    return (uint32_t)(nValue * 10000);
}

// 1.13c D2Common+0x3C390: null room -> FALSE, else the room's level id through the room chain
// (room+0x10 -> +0x58 -> +0x1d0) tested with DRLG_IsTownLevel.
BOOL D2Common_10057_DUNGEON_IsRoomInTown(D2ActiveRoomStrc* pRoom)
{
    return pRoom ? DUNGEON_IsRoomInTown(pRoom) : FALSE;
}

// 1.13c D2Common+0x228D0: ItemsTxt[nItemId].nGemSockets (0x138); out-of-range id yields 0.
uint8_t D2Common_10060_ITEMS_GetAllowedGemSocketsFromItemId(int nItemId)
{
    return ITEMS_GetAllowedGemSocketsFromItemId(nItemId);
}

// 1.13c D2Common+0x330F0: wrapper storing the owner type/GUID (arg1 is the unit, passed in EDI).
void D2Common_10069_UNITS_StoreOwnerTypeAndGUID(D2UnitStrc* pUnit, int nOwnerType, D2UnitGUID nOwnerId)
{
    UNITS_StoreOwnerInfo(pUnit, nOwnerType, (int)nOwnerId);
}

// 1.13c D2Common+0x30670: player-only; pSkills->pLeftSkill's skill id and owner GUID are copied
// into pPlayerData +0x74 / +0x7C. Retail fatals on a null player data or skill list.
void D2Common_10088_UNITS_InitLeftSkill(D2UnitStrc* pUnit)
{
    UNITS_InitLeftSkill(pUnit);
}

// 1.13c D2Common+0x242D0: unit type ITEM only; ItemsTxt[dwClassId].wType[0] (0x11E) indexes
// ItemTypesTxt, whose nClass (0x21) is valid when below 7.
BOOL D2Common_10089_ITEMS_IsClassValid(D2UnitStrc* pUnit)
{
    return ITEMS_IsClassValid(pUnit);
}
