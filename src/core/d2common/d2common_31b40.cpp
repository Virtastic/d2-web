// 1.13c D2Common+0x31B40 UNITS_GetEquippedWeaponFromMonster, image-layout port (32-bit unit address).
// A monster (type 1) with an inventory block at +0x60: the candidate hand item from 0x20700, else 0.
#include <cstdint>
uint32_t D2Common_20700_EquippedHandCandidate(uint32_t pInv, uint32_t* pOutUnit, uint32_t* pOutBodyLoc, uint32_t* pOutIsHeld);
uint32_t D2Common_31B40_GetEquippedWeaponFromMonster(uint32_t pUnit)
{
    if (!pUnit || *(uint32_t*)(uintptr_t)pUnit != 1) return 0;
    uint32_t pInv = *(uint32_t*)(uintptr_t)(pUnit + 0x60);
    if (!pInv) return 0;
    uint32_t cand = pUnit, loc, held;
    return D2Common_20700_EquippedHandCandidate(pInv, &cand, &loc, &held) ? cand : 0;
}
