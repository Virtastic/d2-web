// 1.13c D2Common batch 8: +0x0A090, +0x0E1A0, +0x2EA30, +0x22C20, +0x22970, +0x70610.
// Link-safe naming implementations: the parts pinned by the 1.13c decompile are written out; the
// fatal paths that end in _exit(-1) are modelled as returns, as in the rest of src/core/d2common.
// Prior art: D2MOO 1.10f where the same body exists (named in each block).
#include <cstddef>
#include <cstdint>
#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"
#include "Units/Units.h"
#include "Drlg/D2DrlgPreset.h"
#include "Path/Path.h"

extern "C" {
__attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
__attribute__((weak)) void Ordinal_10024(const void*, uint32_t);
__attribute__((weak)) char DAT_6fdda728[1];
}
// retail fatal: Ordinal_10024(&DAT_6fdda728, Ordinal_10265(code)); _exit(-1). Seams are weak; skip if unlinked.
[[noreturn]] static void d2_fatal(uint32_t code)
{
    if (Ordinal_10265 && Ordinal_10024)
        Ordinal_10024(DAT_6fdda728, Ordinal_10265(code));
    __builtin_trap();
}

static_assert(offsetof(D2ItemsTxt, nDexBonus) == 0x108, "items.txt nDexBonus");
static_assert(offsetof(D2ItemsTxt, nUnique) == 0x129, "items.txt nUnique");
static_assert(sizeof(void*) != 4 || offsetof(D2DynamicPathStrc, dwPathPoints) == 0x28,
              "dynamic path dwPathPoints");   // retail 32-bit offset; wasm64 pointers widen the struct

// 1.13c D2Common+0x0A090 (#10039): act nAct, preset-object slot nUnitId -> the object class id.
// The module table at 0x6FDEE2C8 is 5 act rows of 0x96 (150) dwords, read with no bounds check.
// Same function as D2MOO 1.10f DRLGPRESET_GetObjectIndexFromObjPreset (0x6FD859E0, 5x150 shapes);
// the vendored copy carries the table. UNVERIFIED: the 1.13c table at 0x6FDEE2C8 has not been
// diffed against the vendored 1.10f cells; only the index shape (act*0x96 + id) is pinned.
int D2Common_A090_GetObjectIndexFromObjPreset(uint8_t nAct, int32_t nUnitId)
{
    return DRLGPRESET_GetObjectIndexFromObjPreset(nAct, nUnitId);
}

// 1.13c D2Common+0x0E1A0 (#10075): the 1.13c D2Common_10231. Calls the room/collision impl with the
// destination room and the packed (nX, nY) point, then clears the path's dwPathPoints (+0x28) and
// returns 1. The second argument (unit) is unused. Prior art D2MOO 1.10f
// D2Common_10231/10231_Impl (Step.cpp); 1.13c is the version that forwards nX/nY instead of the
// current coordinates, so the impl is called directly here.
BOOL __stdcall D2Common_10231_Impl(D2DynamicPathStrc* pDynamicPath, D2ActiveRoomStrc* pRoomDest, D2PathPointStrc tDest);

int __stdcall D2Common_E1A0_RepositionPath(D2DynamicPathStrc* pDynamicPath, D2UnitStrc* pUnitUnused,
                                           D2ActiveRoomStrc* pRoom, uint16_t nX, uint16_t nY)
{
    (void)pUnitUnused;
    D2Common_10231_Impl(pDynamicPath, pRoom, D2PathPointStrc{ nX, nY });
    pDynamicPath->dwPathPoints = 0;
    return 1;
}

// 1.13c D2Common+0x2EA30 (#10111): set an object's portal flags. Only a UNIT_OBJECT unit is valid:
// a null unit is fatal 0xFEF and any other type fatal 0xFF0, both followed by _exit(-1) (no return
// to model). Writes the byte at pObjectData+0x05 (nPortalFlags). Prior art D2MOO 1.10f
// UNITS_SetObjectPortalFlags (Units.cpp, 0x6FDC0760).
void D2Common_2EA30_SetObjectPortalFlags(D2UnitStrc* pUnit, uint8_t nPortalFlags)
{
    if (!pUnit || pUnit->dwUnitType != UNIT_OBJECT)
        d2_fatal(pUnit ? 0xFF0 : 0xFEF);
    pUnit->pObjectData->nPortalFlags = nPortalFlags;
}

// 1.13c D2Common+0x22C20 (#10119): items.txt byte at +0x129 (nUnique) for a UNIT_ITEM's class id.
// Null unit is fatal 0x9CE, non-item fatal 0x9CF, missing record fatal 0x9D2; fatal here too.
// Prior art D2MOO 1.10f ITEMS_GetUniqueColumnFromItemsTxt (Items.cpp, 0x6FD9AA70).
uint8_t D2Common_22C20_ItemUniqueColumn(D2UnitStrc* pUnit)
{
    if (!pUnit)
        d2_fatal(0x9CE);
    if (pUnit->dwUnitType != UNIT_ITEM)
        d2_fatal(0x9CF);
    const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(pUnit->dwClassId);
    if (!pRow)
        d2_fatal(0x9D2);
    return pRow->nUnique;
}

// 1.13c D2Common+0x22970 (#10120): items.txt int16 at +0x108 (nDexBonus) for a UNIT_ITEM, except
// that a value of 1 (or a null unit of another type, or a missing record) yields 100.
// Prior art D2MOO 1.10f ITEMS_GetDexBonus (Items.cpp, 0x6FD9D310).
int16_t D2Common_22970_ItemDexBonus(D2UnitStrc* pUnit)
{
    if (pUnit && pUnit->dwUnitType == UNIT_ITEM)
    {
        const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(pUnit->dwClassId);
        if (pRow != nullptr && pRow->nDexBonus != 1)
            return pRow->nDexBonus;
    }
    return 100;
}

// 1.13c D2Common+0x70610 (#10126): export that always fatals. Retail takes Fog message 0x65,
// reports it and calls _exit(-1), so it has no observable output and no return to model.
void D2Common_70610_FatalStub(void)
{
    __builtin_trap(); // retail: Fog message 0x65 report (not wired), then _exit(-1)
}
