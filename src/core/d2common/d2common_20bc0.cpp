// D2Common 0x20BC0 (stdcall inventory, reference item, start node): first item of item type 0x12 in the inventory's grid 2 whose item data
// short (+0x3E) equals the reference's and whose 0x260F0 value exceeds the 0x38A80 total of the data tables' stat record, from `start` on.
#include <cstdint>
#include <cstring>
#include "d2common_itemtables.h"
static uint32_t Rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static int16_t Sh(uint32_t item) {
    uint16_t v = 0;
    if (item && Rd(item, 0) == 4) { const uint32_t d = Rd(item, 0x14); if (d) std::memcpy(&v, (const void*)(uintptr_t)(d + 0x3e), 2); }
    return (int16_t)v;
}
// next node: item data +0x70 (data + 0x5c, then +0x14); a non-item node or no item data stops the walk
static uint32_t Next(uint32_t n) {
    if (Rd(n, 0) != 4) return 0;
    const uint32_t d = Rd(n, 0x14);
    if (!d) return 0;
    return Rd(d, 0x70);
}
// 1.13c D2Common+0x20BC0
uint32_t D2Common_20BC0_InventoryCriteriaScan(uint32_t inv, uint32_t ref, uint32_t start, const D2Common20BC0Env* e) {
    if (!inv || Rd(inv, 0) != 0x1020304) return 0;
    const uint32_t owner = Rd(inv, 8);
    uint32_t layoutPtr = 0;
    alignas(4) uint8_t local[24];
    if (owner && Rd(owner, 0) == 0) {
        const uint32_t id = e->recordId(owner, 0, 1);
        e->layout(id, 0, (uint32_t)(uintptr_t)local);
        layoutPtr = (uint32_t)(uintptr_t)local;
    }
    const uint32_t grid = e->grid(2, inv, layoutPtr);
    if (!grid) return 0;
    uint32_t n = Rd(grid, 0);
    if (start) {
        while (n != start) {
            if (!n) return 0;
            n = Next(n);
        }
    }
    while (n) {
        if (Rd(n, 0) != 4) return 0;
        if (e->itemType(Rd(n, 4)) == 0x12 && Sh(n) == Sh(ref)) {
            int32_t th = 0;
            if (Rd(n, 0x5c) != 0) {
                const int32_t cnt = (int32_t)Rd(e->dataTables, 0xbd4);
                const uint32_t rec = Rd(e->dataTables, 0xbcc) + 0x5898;
                if (cnt > 0x46 && rec) th = e->statTotal(0x460000, rec);
            }
            if (th < e->value(n)) return n;
        }
        n = Next(n);
    }
    return 0;
}
