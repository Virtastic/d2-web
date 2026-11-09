// 1.13c D2Game+0xE0BF0, from the disassembly. S+0x6C flag clear: returns S+0x68 untouched. Otherwise the unit is looked up by
// (type S+0x68, guid S+0x64); missing: returns 0 and leaves S alone. Type 1: +0x48300 gets unit[0x14][0x30] (or 0) in ESI and S
// in EDI, then +0x8FAF0 decides the result. Type 2: unit id 0x151 clears the word behind b168's pointer-to-pointer, else a4a2(unit, 0).
// Any other type returns type - 2. Then S[0x64] = -1, S[0x68] = 6, S[0x6C] = 0.
#include "d2game_e0bf0.h"
#include <cstring>
namespace {
uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
void wr(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
}
uint32_t d2game_e0bf0_port(uint32_t s, uint32_t game, const D2GameE0BF0Env* e) {
    if (!*(const uint8_t*)(uintptr_t)(s + 0x6c)) return rd(s, 0x68);
    const uint32_t unit = e->findUnit(game, rd(s, 0x68), rd(s, 0x64));
    if (!unit) return 0;
    uint32_t ret;
    const uint32_t type = rd(unit, 0);
    if (type == 1) {
        const uint32_t d = rd(unit, 0x14);
        e->detach(d ? rd(d, 0x30) : 0, s, game);
        ret = e->unitCleanup(unit, s, game);
    } else {
        ret = type - 2;
        if (ret == 0) {
            if (rd(unit, 4) == 0x151) {
                const uint32_t q = rd(e->b168(unit), 0);
                if (q) wr(q, 0, 0xffffffffu);
                ret = q;
            } else ret = e->a4a2(unit, 0);
        }
    }
    wr(s, 0x64, 0xffffffffu);
    wr(s, 0x68, 6);
    *(uint8_t*)(uintptr_t)(s + 0x6c) = 0;
    return ret;
}
