// D2Common 0x20E80 (ECX = unit, EBX = item, stack out_a, out_b): a bow or crossbow item sets composite byte 5 or 6 of the weapon-held tables.
// Retail's fatal asserts (null unit 0x13B4, null item 0x13B5, missing items.txt row 0x13D0, then _exit) return here; a capture cannot reach them.
#include <cstdint>
#include <cstring>
#include "d2common_itemtables.h"
static uint32_t Rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static uint8_t* B(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
// 1.13c D2Common+0x20E80
void D2Common_20E80_UpdateItemTables(uint32_t unit, uint32_t outA, uint32_t outB, uint32_t item, const D2Common20E80Env* e) {
    if (!unit || !item) return;
    if (Rd(item, 0) != 4) return;
    const uint32_t data = Rd(item, 0x14);
    if (!data) return;
    const uint8_t kind = B(data)[0x44];
    if (kind != 4 && kind != 5) return;
    const uint32_t inv = Rd(unit, 0x60);
    uint32_t slotOut = outA;
    const int32_t code = e->weaponClass(unit, inv, &slotOut, -1, 1);
    const uint8_t slot = e->slot(item);
    if (code == 0x20776278) {                 // "xbw "
        if ((slot != 5 && slot != 6) || !inv) return;
        const uint32_t c = e->component(inv, 5);
        if (!c) return;
        const uint32_t rec = e->itemsTxt(Rd(c, 4));
        if (!rec) return;                     // retail: fatal 0x13D0
        const int32_t n = (int32_t)e->composite(Rd(rec, 0x90), Rd(rec, 0x80));
        if (n > 0) { B(outA)[6] = (uint8_t)n; return; }
        B(outA)[6] = 0xff; B(outB)[6] = 0xff;
    } else if (code == 0x20776f62 && slot == 6) {   // "bow "
        uint32_t tmp = 0;
        const uint32_t rec = e->itemByCode(0x2074696c, &tmp);   // "lit "
        if (!rec) return;
        const uint32_t n = e->composite(Rd(rec, 0x90), Rd(rec, 0x80));
        if (n == 0) { B(outA)[5] = 0xff; B(outB)[5] = 0xff; return; }
        B(outA)[5] = (uint8_t)n;
    }
}
