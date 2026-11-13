// 1.13c D2Game+0x3FD30 -- monster-mode callback: when the unit has flag bit 10, an owner (0xE03A0) that 0xDFA90 does not reject, and 0xB28E(unit) equals row +0x38,
// heals the owner: owner value 8 (0xA42A, >>8) is raised by 0xAC9A's share of the owner maximum (0xA4EA >> 8, 0x14D0 scaled, capped at it) when below that maximum,
// stored back <<8 through 0xA472; the skill row's +0x138 goes to 0xAC9A; monster row short +0x36, when 1..count (dt +0xBC0)-1, goes to 0xA83E. Returns 0x3F320(game) with
// EBX = unit on every path past the lookups; 2 when the unit, monster row or skill row (skill id 0xB26A(unit) against dt +0xBA0, 0x23C stride at dt +0xB98) is missing.
// fastcall: ECX = game, EDX = unit
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f3fd30(NE& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    if (!unit) return 2;
    const int32_t id = (int32_t)e.rd32(unit + 4);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xb6c)) return 2;
    const uint32_t mrow = e.rd32(dt + 0xb64) + (uint32_t)id * 0x1a4u;
    if (!mrow) return 2;
    const int32_t sid = (int32_t)e.imp(0xB26A, unit);
    if (sid < 0 || sid >= (int32_t)e.rd32(dt + 0xba0)) return 2;
    const uint32_t srow = (uint32_t)sid * 0x23cu + e.rd32(dt + 0xb98);
    if (!srow) return 2;
    if (e.rd32(unit + 0xc8) >> 10 & 1) {
        const uint32_t owner = e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98));
        if (owner && !e.h(0xDFA90, owner)) {
            const uint32_t b0 = e.imp(0xB0FC, unit);
            if (e.imp(0xB28E, unit) == e.rd32(mrow + 0x38)) {
                int32_t cur = (int32_t)e.imp(0xA42A, owner, 8, 0) >> 8;
                const int32_t mx = (int32_t)e.imp(0xA4EA, owner) >> 8;
                if (cur < mx) {
                    const uint32_t g = e.imp(0xAC9A, owner, e.rd32(srow + 0x138), (uint32_t)sid, b0);
                    cur += (int32_t)e.h(0x14D0, g, 100, (uint32_t)mx);
                    if (cur >= mx) cur = mx;
                    e.imp(0xA472, owner, 8, (uint32_t)cur << 8, 0);
                    const int32_t s = (int16_t)e.rd16(mrow + 0x36);
                    if (s > 0 && s < (int32_t)e.rd32(dt + 0xbc0)) e.imp(0xA83E, owner, (uint32_t)s, 0);
                }
            }
        }
    }
    return e.h(0x3F320, 0, 0, 0, unit, 0, game);
}
}
