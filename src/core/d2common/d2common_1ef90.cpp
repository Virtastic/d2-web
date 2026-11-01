// 1.13c D2Common+0x1EF90 (retail-layout, 32-bit image): the item cell at (x, y) of one inventory grid page.
// stdcall(inv, x, y, pOut1, pOut2, rec, page), ret 0x1C. The grid is fetched with 0x1E1B0 at index page+2 using the 2-byte
// size pair from the inventory.txt layout (0x81FB0); x in [0, width] and y in [0, height] (inclusive, retail quirk) select
// cells[width*y + x]; a non-null cell is returned and its two words from 0x1050 / 0x1080 are stored through the out-pointers.
#include <cstdint>
#include "inv_place.h"
struct D2Common1EF90Env { uint32_t (*cell1050)(uint32_t cell); uint32_t (*cell1080)(uint32_t cell); const InvEnv* inv; };
extern "C" uint32_t D2Common_1EF90_CellAt(const D2Common1EF90Env* e, uint32_t inv, int32_t x, int32_t y, uint32_t* o1, uint32_t* o2, int32_t rec, uint8_t page)
{
    auto P = [](uint32_t a) { return (uint8_t*)(uintptr_t)a; };
    if (!inv || *(const uint32_t*)P(inv) != 0x1020304) return 0;
    uint8_t info[24];
    if (!D2Common_81FB0_GetLayout(*e->inv, rec, 0, info)) return 0;
    uint32_t g = D2Common_1E1B0_GetGrid(*e->inv, inv, (int32_t)page + 2, (uint32_t)(uintptr_t)info);
    if (!g) return 0;
    int32_t w = P(g)[8], h = P(g)[9];
    if (x < 0 || x > w || y < 0 || y > h) return 0;
    uint32_t cell = ((const uint32_t*)P(*(const uint32_t*)(P(g) + 0xC)))[w * y + x];
    if (!cell) return 0;
    *o1 = e->cell1050(cell);
    *o2 = e->cell1080(cell);
    return cell;
}
