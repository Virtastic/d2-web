// 1.13c D2Common+0x7F670 (#10329) the decompile  (callee sub_1000 inlined:)
#include <cstdint>
#include "Units/Units.h"
#include "Units/Player.h"
// image offsets 0x150..0x15C = unk0xA8[42..45] (image 0xA8 + 4*42 = 0x150); the build is 64-bit so offsetof is not the image value.
static_assert(sizeof(D2PlayerDataStrc::unk0xA8) / sizeof(uint32_t) == 46, "image: array ends at 0x160");
// Player unit with player data: pPlayerData+0x150 = 1, +0x154/0x158/0x15C = a, b, c.
void D2Common_7F670_SetPlayerNotice(D2UnitStrc* pUnit, uint32_t a, uint32_t b, uint32_t c)
{
    if (!pUnit || pUnit->dwUnitType != 0 || !pUnit->pPlayerData) return;
    uint32_t* p = &pUnit->pPlayerData->unk0xA8[42];
    p[0] = 1; p[1] = a; p[2] = b; p[3] = c;
}
