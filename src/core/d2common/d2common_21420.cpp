// D2Common 0x21420 (stdcall unit, out_a, out_b): walk the item nodes of the unit's inventory (unit+0x60, head magic 0x1020304, first node head+0xC,
// next node = node item data +0x64) and refresh the 16-byte composite (out_a) and color (out_b) tables from each node of type 4 whose
// +0x10 is 1: item data kind 1/4/5 calls 0x21300 unless the items.txt row's type (+0x11E) has bit 11 set in the data tables' per-type masks
// (and 0x4C or more types exist); kind 3 runs 0x1E420 for a context, then for slots 1,2,3,4,8,9 asks 0x43710 for a type row, 0x1FB50 for the
// composite id and 0x251C0 for the color. Retail's fatal asserts (class past the items table 0x1447, no items.txt base 0x1EF, no type row 0x150,
// then _exit) return here; a capture cannot reach them.
#include <cstdint>
#include <cstring>
#include "d2common_itemtables.h"
static uint32_t Rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static uint8_t* B(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
// 1.13c D2Common+0x21420
void D2Common_21420_RefreshItemNodes(uint32_t unit, uint32_t outA, uint32_t outB, const D2Common21420Env* e) {
    const uint32_t head = Rd(unit, 0x60);
    if (!head || Rd(head, 0) != 0x1020304) return;
    uint32_t node = Rd(head, 0xc);
    while (node) {
        if (Rd(node, 0) == 4 && Rd(node, 0x10) == 1) {
            const uint32_t cls = Rd(node, 4);
            if (cls >= e->itemCount || !e->itemTable) return;   // retail: fatal 0x1447 / 0x1EF
            const uint32_t rec = cls * 0x1a8 + e->itemTable;
            const uint32_t data = Rd(node, 0x14);
            if (data) {
                const uint8_t kind = B(data)[0x44];
                if (kind == 1 || kind == 4 || kind == 5) {
                    int16_t t; std::memcpy(&t, B(rec) + 0x11e, 2);
                    const int32_t type = t, typeCount = (int32_t)Rd(e->dataTables, 0xbfc);
                    bool skip = false;
                    if (type >= 0 && type < typeCount && typeCount > 0x4b) {
                        const int32_t off = (int32_t)Rd(e->dataTables, 0xc00) * type;
                        skip = (Rd(Rd(e->dataTables, 0xc04) + (uint32_t)off * 4u, 8) & e->bitMask11) != 0;
                    }
                    if (!skip) e->refresh(unit, outA, outB, node);
                } else if (kind == 3) {
                    uint8_t ctx[12];
                    e->context(node, (uint32_t)(uintptr_t)ctx);
                    for (uint32_t i = 0; i < 0x10; i++) {
                        if (i != 1 && i != 2 && i != 3 && i != 4 && i != 8 && i != 9) continue;
                        const uint32_t trec = e->typeIndex(i, (uint32_t)(uintptr_t)ctx) * 0x34 + e->typesTable;
                        if (!trec) return;                       // retail: fatal 0x150
                        uint32_t n = 0;
                        if ((int32_t)Rd(node, 4) > 0) {
                            const uint32_t v = Rd(trec, 0x20);
                            n = e->composite(v, v);
                        }
                        if (n == 0) { B(outA)[i] = 0xff; B(outB)[i] = 0xff; continue; }
                        uint8_t* col = B(outB) + i;
                        B(outA)[i] = (uint8_t)n;
                        if (e->color(unit, node, (uint32_t)(uintptr_t)col, 0) == 0) *col = 0xff; else ++*col;
                    }
                }
            }
        }
        if (Rd(node, 0) != 4) return;
        const uint32_t d = Rd(node, 0x14);
        if (!d) return;
        node = Rd(d, 0x64);
    }
}
