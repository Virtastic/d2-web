// 1.13c D2Game+0xAD9C0 -- ECX game, EDX unit, five stack words (ret 0x14), a0 = an optional unit (its id at +0xC) is the only one read. Always returns 0.
// Unit type must be 0. When the byte at +0x6C is set, or 0xA6A6([+0x60]) answers, or the pData (0xA4C0) word +0x4C is non-zero: with the byte clear return 0; else
// x = 0xE03A0(ECX game, EDX [+0x64], EAX [+0x68]) must be a record with [0] == 2 and [1] == 0x10B; then [+0x64] = -1, [+0x68] = 6, byte [+0x6C] = 0, 0xD4570(EAX unit),
// 0x8B340(AL 0x11, pData+0x9C). Then (also when none of the three held) 0xDFAE0(EAX unit, 4, a0 ? a0[3] : -1), 0x8A3E0(EAX pData+0x9C, {0x77, 0x15}, 2), 0xD4570(EAX unit).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fad9c0(GE& e, uint32_t game, uint32_t unit, uint32_t a0)
{
    if (!unit || e.rd32(unit) != 0) return 0;
    bool enter = true;
    if (!e.rd8(unit + 0x6C)) {
        if (!e.h(0xA6A6, 0, 0, 0, 0, 0, e.rd32(unit + 0x60))) {
            const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
            enter = e.rd32(pd + 0x4C) != 0;
        }
    }
    if (enter) {
        if (!e.rd8(unit + 0x6C)) return 0;
        const uint32_t x = e.h(0xE03A0, e.rd32(unit + 0x68), game, e.rd32(unit + 0x64), 0, 0);
        if (!x || e.rd32(x) != 2 || e.rd32(x + 4) != 0x10B) return 0;
        e.wr32(unit + 0x64, 0xFFFFFFFFu); e.wr32(unit + 0x68, 6); e.wr8(unit + 0x6C, 0);
        e.h(0xD4570, unit);
        const uint32_t v = e.rd32(unit) == 0 ? e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C) : 0;
        e.h(0x8B340, 0x11, 0, 0, 0, 0, v);
    }
    const uint32_t id = a0 ? e.rd32(a0 + 0xC) : 0xFFFFFFFFu;
    e.h(0xDFAE0, unit, 0, 0, 0, 0, 4, id);
    const uint32_t cid = e.rd32(unit) == 0 ? e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C) : 0;
    uint32_t pkt = 0x1577;
    e.h(0x8A3E0, cid, 0, 0, 0, 0, (uint32_t)(uintptr_t)&pkt, 2);
    e.blk(0x8A3E0, (uint32_t)(uintptr_t)&pkt, 1);
    e.h(0xD4570, unit);
    return 0;
}
}
