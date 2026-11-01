// 1.13c D2Common+0x1E7B0 (retail-layout, 32-bit image): copy the cell array of a unit's main inventory grid into a trade inventory.
// Register convention: EAX = pUnit, ECX = pTradeInventory, plain ret. The grid size pair comes from the inventory.txt layout (0x81FB0)
// of the unit's record (0x2FB60 with page 0, flag 1); both grids are index 2 (the main page). Both must exist, be non-empty and
// match in width and height and own a cell array; then width*height dwords are copied and 1 is returned, else 0.
#include <cstdint>
#include <cstring>
#include "inv_place.h"
struct D2Common1E7B0Env { int32_t (*unitRecord)(uint32_t unit); const InvEnv* inv; };
extern "C" uint32_t D2Common_1E7B0_CopyGrid(const D2Common1E7B0Env* e, uint32_t unit, uint32_t trade)
{
    auto P = [](uint32_t a) { return (uint8_t*)(uintptr_t)a; };
    if (!trade || *(const uint32_t*)P(trade) != 0x1020304) return 0;
    uint8_t info[24];
    if (!D2Common_81FB0_GetLayout(*e->inv, e->unitRecord(unit), 0, info)) return 0;
    uint32_t tg = D2Common_1E1B0_GetGrid(*e->inv, trade, 2, (uint32_t)(uintptr_t)info);
    if (!tg) return 0;
    uint32_t ui = *(const uint32_t*)(P(unit) + 0x60);
    if (!ui || *(const uint32_t*)P(ui) != 0x1020304) return 0;
    uint32_t ug = D2Common_1E1B0_GetGrid(*e->inv, ui, 2, (uint32_t)(uintptr_t)info);
    if (!ug) return 0;
    uint8_t w = P(ug)[8], h = P(ug)[9];
    if (!w || !h || w != P(tg)[8] || h != P(tg)[9]) return 0;
    uint32_t src = *(const uint32_t*)(P(ug) + 0xC), dst = *(const uint32_t*)(P(tg) + 0xC);
    if (!src || !dst) return 0;
    memcpy(P(dst), P(src), (uint32_t)w * h * 4);
    return 1;
}
