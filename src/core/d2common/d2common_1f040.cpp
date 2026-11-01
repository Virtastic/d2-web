// 1.13c D2Common+0x1F040 (retail-layout, 32-bit image): can the item be put at (x, y) of one inventory page, swapping with at most one item?
// stdcall(inv, item, x, y, rec, &exchange, page, checkCube), ret 0x20. The item size comes from 0x231A0 (item, &w, &h, file, line); the grid
// is fetched with 0x1E1B0 at index page+2 using the 0x81FB0 layout of `rec`; the rectangle must lie inside the grid, and the cells under it
// may hold at most one distinct item (last one seen is stored through &exchange). With checkCube set, any item whose items.txt code
// (row +0x80) is "box " fails the call at once.
#include <cstdint>
#include "inv_place.h"
struct D2Common1F040Env { void (*dims)(uint32_t item, uint32_t* w, uint32_t* h); uint32_t (*itemRec)(uint32_t classId); const InvEnv* inv; };
extern "C" uint32_t D2Common_1F040_CanItemsBeExchangedRaw(const D2Common1F040Env* e, uint32_t inv, uint32_t item, int32_t x, int32_t y,
                                                          int32_t rec, uint32_t* pExch, uint32_t page, int32_t checkCube)
{
    auto P = [](uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); };
    if (!inv || *reinterpret_cast<const uint32_t*>(P(inv)) != 0x1020304 || !item || *reinterpret_cast<const uint32_t*>(P(item)) != 4) return 0;
    if ((page & 0xFF) == 0xFF || x < 0 || y < 0) return 0;
    uint32_t w = 0, h = 0;
    e->dims(item, &w, &h);
    if (!(w & 0xFF) || !(h & 0xFF)) return 0;
    uint8_t info[24];
    D2Common_81FB0_GetLayout(*e->inv, rec, 0, info);
    const uint32_t g = D2Common_1E1B0_GetGrid(*e->inv, inv, static_cast<int32_t>(page & 0xFF) + 2, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(info)));
    if (!g) return 0;
    const int32_t yMax = static_cast<int32_t>(h & 0xFF) + y, xMax = static_cast<int32_t>(w & 0xFF) + x;
    if (yMax > P(g)[9] || xMax > P(g)[8]) return 0;
    uint32_t last = 0, count = 0;
    const uint32_t* cells = reinterpret_cast<const uint32_t*>(P(*reinterpret_cast<const uint32_t*>(P(g) + 0xC)));
    for (int32_t yy = y; yy < yMax; ++yy)
        for (int32_t xx = x; xx < xMax; ++xx) {
            const uint32_t cell = cells[P(g)[8] * yy + xx];
            if (!cell) continue;
            *pExch = cell;
            const uint32_t row = e->itemRec(*reinterpret_cast<const uint32_t*>(P(cell) + 4));
            if (checkCube && *reinterpret_cast<const uint32_t*>(P(row) + 0x80) == 0x20786f62) return 0;
            if (cell != last) { ++count; last = cell; }
        }
    if (count > 1) { *pExch = 0; return 0; }
    return 1;
}
