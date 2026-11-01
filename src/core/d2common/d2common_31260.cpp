// D2Common 0x31260 (#10921), retail layout. Units: +0 type, +4 class, +0x14 data, +0x60
// inventory; item data +0x44 = type class byte, +0x18 = flags. A class 4/5 item yields 6 when the unit's inventory
// (magic 0x01020304) holds two usable equipped units (slot block +0x10 / +0x14, an item flagged 0x100 or 0x4000 counts as
// none), neither a two-handed item, the unit can dual-wield, and the inventory's designated item is not this one.
// Otherwise the items.txt row byte +0x115. Null / non-item / missing row are fatal (0x635 / 0x636 / 0x639): 0 here.
#include <cstdint>
#include <cstring>
#include "d2common_31260.h"
static uint32_t Rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static uint8_t Rb(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
static uint32_t Slot(uint32_t inv, uint32_t off, const D2Common31260Env* env) {
    if (!inv || Rd(inv, 0) != 0x1020304) return 0;
    const uint32_t e = env->slots();
    if (!e) return 0;
    const uint32_t p = Rd(Rd(e, 0xc), off);
    if (!p) return 0;
    if (Rd(p, 0) == 4 && Rd(p, 0x14) && (Rd(Rd(p, 0x14), 0x18) & 0x100)) return 0;
    if (Rd(p, 0) == 4 && Rd(p, 0x14) && (Rd(Rd(p, 0x14), 0x18) & 0x4000)) return 0;
    return p;
}
// 1.13c D2Common+0x31260
extern "C" uint32_t D2Common_31260_ItemTypeClass(uint32_t unit, uint32_t item, const D2Common31260Env* env) {
    uint8_t cls = 0;
    if (item && Rd(item, 0) == 4 && Rd(item, 0x14)) cls = Rb(Rd(item, 0x14), 0x44);
    if (cls == 5 || cls == 4) {
        const uint32_t inv = Rd(unit, 0x60);
        const uint32_t a = Slot(inv, 0x10, env);
        const uint32_t b = Slot(inv, 0x14, env);
        if (a && b && (Rd(a, 0) != 4 || !env->twoHanded(Rd(a, 4))) && (Rd(b, 0) != 4 || !env->twoHanded(Rd(b, 4))) &&
            env->dualWield(unit) && env->invItem(inv) != item)
            return 6;
    }
    if (!item || Rd(item, 0) != 4) return 0;
    const uint32_t r = env->itemsRow(Rd(item, 4));
    return r ? Rb(r, 0x115) : 0;
}
