// D2Common 0x21300 (stdcall unit, out_a, out_b, item): map one item into the 16-byte composite and color tables.
// Retail's fatal asserts (null item or class past the table 0x1EF / 0x140D, then _exit) return here; a capture cannot reach them.
#include <cstdint>
#include <cstring>
#include "d2common_itemtables.h"
static uint32_t Rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static uint8_t* B(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
// 1.13c D2Common+0x21300
void D2Common_21300_UnitItemSlots(uint32_t unit, uint32_t outA, uint32_t outB, uint32_t item, const D2Common21300Env* e) {
    const uint32_t inv = Rd(unit, 0x60);
    if (!inv) return;
    const uint32_t c5 = e->component(inv, 5);
    const uint32_t c6 = e->component(inv, 6);
    const uint32_t cls = item ? Rd(item, 4) : 0xffffffffu;
    if (cls >= e->itemCount || !e->itemTable) return;   // retail: fatal 0x1EF / 0x140D
    const uint32_t rec = cls * 0x1a8 + e->itemTable;
    uint8_t slot = e->slot(item);
    const uint32_t n = e->composite(Rd(rec, 0x90), Rd(rec, 0x80));
    if (n == 0) {
        if (slot < 0x10) { B(outA)[slot] = 0xff; B(outB)[slot] = 0xff; }
        return;
    }
    if (item == c5) slot = 5;
    else if (item == c6) slot = 6;
    else if (slot >= 0x10) return;
    B(outA)[slot] = (uint8_t)n;
    e->update(unit, item, outA, outB);
    uint8_t* col = B(outB) + slot;
    if (e->color(unit, item, (uint32_t)(uintptr_t)col, 0) == 0) *col = 0xff; else ++*col;
}
