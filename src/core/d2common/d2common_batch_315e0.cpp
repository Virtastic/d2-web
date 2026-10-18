// 1.13c D2Common implementation batch (rows 0x315E0, 0x32C10, 0x82EC0, 0x46600, 0x23130).
// Each body delegates to the vendored D2MOO 1.10f function whose 1.10f/1.13c behaviour was matched
// line by line against the decompile; 0x32C10 has no 1.10f twin and is written here.
#include <cstdint>
#include "D2DataTbls.h"
#include "D2ItemMods.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "Units/UnitRoom.h"
#include "Units/Units.h"
#include "DataTbls/OverlayTbls.h"

// 1.13c D2Common+0x315E0: null unit or null room -> FALSE; otherwise the unit's coordinates and
// size are tested against the point (nX, nY) with size 2 in the unit's room, using nFlags.
extern "C"
BOOL __stdcall D2Common_10686_UNITS_TestCollisionByCoordinates(D2UnitStrc* pUnit, int nX, int nY, int nFlags)
{
    return UNITS_TestCollisionByCoordinates(pUnit, nX, nY, nFlags);
}

// 1.13c D2Common+0x32C10: stores an overlay id in the unit's overlay stat list (filter 0x80).
// Negative ids and ids at or above OverlayTxt's record count are ignored. When the unit has no
// extended stat list or no overlay list yet, one is allocated with the unit's pool/type/GUID and
// posted to the unit. STATLIST_UNK_0x100 is then set on the stat list ex, the list's
// STAT_UNIT_DOOVERLAY (0xB2) is set to the id & 0xFFFF and the unit is refreshed.
// No 1.10f twin; the body follows the decompile stores directly.
extern "C"
void __stdcall D2Common_10689_UNITS_SetOverlay(D2UnitStrc* pUnit, int nOverlayId)
{
    if (nOverlayId < 0 || nOverlayId >= sgptDataTables->nOverlayTxtRecordCount)
    {
        return;
    }

    D2StatListStrc* pOverlayStatList = NULL;
    if (pUnit->pStatListEx && STATLIST_IsExtended(pUnit->pStatListEx))
    {
        pOverlayStatList = STATLIST_GetStatListFromStatListExAndFlag(pUnit->pStatListEx, STATLIST_OVERLAY);
    }

    if (!pOverlayStatList)
    {
        pOverlayStatList = STATLIST_AllocStatList(pUnit->pMemoryPool, STATLIST_OVERLAY, 0, pUnit->dwUnitType, pUnit->dwUnitId);
        D2COMMON_10475_PostStatToStatList(pUnit, pOverlayStatList, TRUE);
    }

    if (pUnit->pStatListEx)
    {
        pUnit->pStatListEx->dwFlags |= STATLIST_UNK_0x100;
    }

    STATLIST_SetStat(pOverlayStatList, STAT_UNIT_DOOVERLAY, nOverlayId & 0xFFFF, 0);
    UNITROOM_RefreshUnit(pUnit);
}

// 1.13c D2Common+0x82EC0: OverlayTxt[nOverlayId].nPreDraw (row stride 0x84, field at 0x48);
// negative or at/above the record count, or a null row, yields 0.
extern "C"
int __stdcall D2Common_10693_DATABLS_GetPreDrawFromOverlayTxt(int nOverlayId)
{
    return DATABLS_GetPreDrawFromOverlayTxt(nOverlayId);
}

// 1.13c D2Common+0x46600: searches an item (and, recursively, the items it contains) for a
// charged skill: nulls both outputs first, requires a UNIT_ITEM whose skill id is a valid SkillsTxt
// record and a level in 1..63, then walks the 0x40-flag stat list chain for STAT_ITEM_CHARGED_SKILL
// in the layer packed from skill id/level. On a hit writes the value & 0xFF and the owning stat list
// and returns TRUE.
extern "C"
BOOL __stdcall D2Common_10699_ITEMMODS_GetItemCharges(D2UnitStrc* pItem, int nSkillId, int nSkillLevel, int* pValue, D2StatListStrc** ppStatList)
{
    return ITEMMODS_GetItemCharges(pItem, nSkillId, nSkillLevel, pValue, ppStatList);
}

// 1.13c D2Common+0x23130: for a UNIT_ITEM the ItemTypesTxt row selected by the item's ItemsTxt
// type yields nBodyLoc1/nBodyLoc2 (0xA/0xB); a non-item unit or an out-of-range/null row writes
// zeros to both outputs.
extern "C"
void __stdcall D2Common_10700_ITEMS_GetAllowedBodyLocations(D2UnitStrc* pItem, uint8_t* pBodyLoc1, uint8_t* pBodyLoc2)
{
    ITEMS_GetAllowedBodyLocations(pItem, pBodyLoc1, pBodyLoc2);
}
