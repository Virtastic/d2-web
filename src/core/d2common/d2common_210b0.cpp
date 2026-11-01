// D2Common 0x210B0 (#10277, stdcall inv, item, start; ret 0xC), retail layout. Inventory: +0 magic 0x01020304, +8 owner
// (+0 type). Items: +0 type (4), +0x14 data, +0x5C stat list; data +0x70 is the next item. A player owner fills a local
// page key through 0x2FB60/0x81FB0; page 2 of the inventory (0x1E1B0) heads the walk. With a start item the walk first
// runs to it (absent: 0). Each item that 0x26930 accepts returns when its quantity (stat 0x46; 0 without a stat list, with a
// table count at or below 0x46, or for a zero row address) is below 0x260F0's maximum.
#include <cstdint>
#include <cstring>
#include "d2common_210b0.h"
static uint32_t Rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static uint32_t Next(uint32_t it) {
    if (Rd(it) != 4) return 0;
    const uint32_t d = Rd(it + 0x14);
    return d ? Rd(d + 0x70) : 0;
}
// 1.13c D2Common+0x210B0
extern "C" uint32_t D2Common_210B0_FindStack(uint32_t inv, uint32_t item, uint32_t start, const D2Common210B0Env* env) {
    if (!inv || Rd(inv) != 0x01020304u) return 0;
    uint32_t key[6] = {};
    uint32_t k = 0;
    const uint32_t owner = Rd(inv + 8);
    if (owner && Rd(owner) == 0) {
        k = (uint32_t)(uintptr_t)key;
        env->pageKey(env->ownerPlayer(owner, 0, 1), 0, k);
    }
    const uint32_t pg = env->page(inv, k);
    if (!pg) return 0;
    uint32_t it = Rd(pg);
    if (start) {
        while (it != start) { if (!it) return 0; it = Next(it); }
    }
    for (; it; it = Next(it)) {
        if (!env->canStack(it, item)) continue;
        const uint32_t list = Rd(it + 0x5c);
        int32_t q = 0;
        if (list && (int32_t)Rd(env->dataTables + 0xbd4) > 0x46) {
            const uint32_t desc = Rd(env->dataTables + 0xbcc) + 0x5898;
            if (desc) q = env->statValue(list, desc);
        }
        if (q < env->maxStack(it)) return it;
    }
    return 0;
}
