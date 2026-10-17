// D2Common+0x1000 and +0x7F670
#include <cstddef>
#include <cstdint>
#include "Units/Units.h"
#include "Units/Player.h"
// Image offsets 0x150..0x15C = unk0xA8[42..45] (image 0xA8 + 4*42 = 0x150); the build is 64-bit so
// offsetof is not the image value. The probe translates.
static_assert(sizeof(D2PlayerDataStrc::unk0xA8) / sizeof(uint32_t) == 46, "image: array ends at 0x160");
static_assert(offsetof(D2PlayerDataStrc, unk0xA8) + 42 * sizeof(uint32_t) > 0x150,
              "wasm64 puts unk0xA8 past its image offset; the FN probe's skew depends on this");

// 1.13c D2Common+0x1000: the unit in EAX, a/b/c on the stack. Re-applies 0x7F670's guards and stores
// the notice record (1, a, b, c) at playerdata+0x150..0x15C. Nothing on any early exit.
void D2Common_1000_SetPlayerNotice(D2UnitStrc* pUnit, uint32_t a, uint32_t b, uint32_t c)
{
    if (!pUnit || pUnit->dwUnitType != 0 || !pUnit->pPlayerData) return;
    uint32_t* p = &pUnit->pPlayerData->unk0xA8[42];
    p[0] = 1; p[1] = a; p[2] = b; p[3] = c;
}

// 1.13c D2Common+0x7F670 (#10329): player unit with player data: +0x150 = 1, then 0x1000 stores a, b, c.
void D2Common_7F670_SetPlayerNotice(D2UnitStrc* pUnit, uint32_t a, uint32_t b, uint32_t c)
{
    if (!pUnit || pUnit->dwUnitType != 0) return;
    if (pUnit->pPlayerData) pUnit->pPlayerData->unk0xA8[42] = 1;
    D2Common_1000_SetPlayerNotice(pUnit, a, b, c);
}
