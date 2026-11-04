// 1.13c D2Game+0x6B430: place the unit's marked code item, else a rotating colour item. Register convention: ECX = ctx {+0 game, +8 unit}, no result.
// For each item of the unit's room list (unit[0x60]) via 0xB036(list, 0x14, cur): rec = 0xA53E(item ? item[4] : -1); a rec[0xBC] code other than "non "
// is resolved by 0xAE74 (-1 is Fog fatal line 0x823) and offered to 0x6B070(game, unit, idx, item); the first nonzero ends the scan (one more 0xB036 call
// is made, as retail does). With none placed the 64-bit seed at unit[0x20] becomes seed * 0x6AC690C5 + unit[0x24] and its low word % 6 picks "cgw ",
// "cgr ", "cgg ", "cgb ", "cgy " or "cgv " for 0xAE74 (-1: fatal 0x84C) and 0x6B070(game, unit, idx, 0).
#include <cstdint>
#include <cstring>
#include "d2game_6b430.h"

static inline uint32_t rd32(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
static inline void wr32(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(p + o), &v, 4); }

// 1.13c D2Game+0x6B430
void D2Game_6B430(uint32_t pCtx, const D2GameCodeItemEnv* pEnv)
{
    const uint32_t list = rd32(rd32(pCtx, 8), 0x60);
    uint32_t it = pEnv->nextItem(list, 0x14, 0);
    bool placed = false;
    while (it) {
        const uint32_t rec = pEnv->itemRec(rd32(it, 4));
        const uint32_t code = rd32(rec, 0xBC);
        if (code != 0x206E6F6Eu) {
            const int32_t idx = pEnv->codeIdx(code);
            if (idx == -1) { pEnv->fatal(0x823); return; }
            if (pEnv->place(rd32(pCtx, 0), rd32(pCtx, 8), (uint32_t)idx, it)) { placed = true; it = 0; }
        }
        it = pEnv->nextItem(list, 0x14, it);
        if (placed) return;
    }
    const uint32_t u = rd32(pCtx, 8);
    const uint64_t v = (uint64_t)rd32(u, 0x20) * 0x6AC690C5ull + rd32(u, 0x24);
    wr32(u, 0x20, (uint32_t)v); wr32(u, 0x24, (uint32_t)(v >> 32));
    static const uint32_t kCodes[6] = { 0x20776367u, 0x20726367u, 0x20676367u, 0x20626367u, 0x20796367u, 0x20766367u };
    const int32_t idx = pEnv->codeIdx(kCodes[(uint32_t)v % 6u]);
    if (idx == -1) { pEnv->fatal(0x84C); return; }
    pEnv->place(rd32(pCtx, 0), rd32(pCtx, 8), (uint32_t)idx, 0);
}
