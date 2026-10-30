// 1.13c D2Common implementation batch 15 (rows 0x37520, 0x2F910, 0x23450).
// Bodies follow the 1.13c decompile; the fatal paths that end in _exit(-1) are modelled as returns, as
// in the rest of src/core/d2common.
#include <cstddef>
#include <cstdint>
#include "D2Environment.h"
#include "Drlg/D2DrlgDrlg.h"
#include "D2Items.h"
#include "Units/Units.h"

static_assert(offsetof(D2DrlgEnvironmentStrc, nRed) == 0x18, "environment nRed");
static_assert(offsetof(D2DrlgEnvironmentStrc, nGreen) == 0x19, "environment nGreen");
static_assert(offsetof(D2DrlgEnvironmentStrc, nBlue) == 0x1A, "environment nBlue");

// 1.13c D2Common+0x37520 (#10189): act -> day/night environment -> RGB light color.
// The decompile checks pAct for null (fatal in retail) and reads the environment pointer at act+0x04,
// i.e. the 1.13c D2DrlgActStrc order (eLevelIdFirst, pEnvironment). Our engine allocates the vendored
// 1.10f D2DrlgActStrc (DUNGEON_AllocAct), so the member is used and the RGB offsets are pinned above.
// Prior art: D2MOO 1.10f ENVIRONMENT_GetLightColorFromAct (ordinal 10927), same body.
extern "C" void D2Common_1B0D_Exit(int32_t);
void D2Common_37520_GetLightColorFromAct(D2DrlgActStrc* pAct, uint8_t* pRed, uint8_t* pGreen, uint8_t* pBlue)
{
    if (!pAct)
    {
        D2Common_1B0D_Exit(-1); // retail: fatal 0x547, _exit(-1)
    }

    D2DrlgEnvironmentStrc* pEnvironment = pAct->pEnvironment;
    if (pEnvironment)
    {
        *pRed = pEnvironment->nRed;
        *pGreen = pEnvironment->nGreen;
        *pBlue = pEnvironment->nBlue;
    }
}

// 1.13c D2Common+0x2F910 (#10192): direction (0..63) from a unit's current tile to (nX, nY).
// The decompile inlines exactly the vendored 1.10f UNITS_GetDirectionToCoords: object/item/tile read
// the static path coords, the rest the dynamic path position (0 when it has no dynamic path), and the
// 16.16-centered line direction comes from PATH_ComputeDirection (callee 0xCF40 is its
// PATH_GetDirectionVector). Prior art: D2MOO 1.10f UNITS_GetDirectionToCoords (ordinal 10418).
int __stdcall D2Common_2F910_GetDirectionToCoords(D2UnitStrc* pUnit, int nX, int nY)
{
    return UNITS_GetDirectionToCoords(pUnit, nX, nY);
}

// 1.13c D2Common+0x23450 (#10208): an item unit's itemTypes.txt nVarInvGfx count (+0x23 of the
// 0xE4-byte row). Non-item/null -> 0; the type index is items.txt wType[0] via 0x230F0
// (sign-extended int16, 0 when out of range), then 0 <= nType < nItemTypesTxtRecordCount.
// The decompile is the vendored 1.10f ITEMS_GetVarInvGfxCount (ordinal 10789) with the rows at
// sgptDataTables +0xBF8 and count +0xBFC. Prior art: D2MOO 1.10f ITEMS_GetVarInvGfxCount.
uint8_t __stdcall D2Common_23450_GetVarInvGfxCount(D2UnitStrc* pItem)
{
    return ITEMS_GetVarInvGfxCount(pItem);
}
