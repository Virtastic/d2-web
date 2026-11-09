// 1.13c D2Game+0xE1570, from the disassembly. unit == 0 is fatal 0x78B. d = unit[0x14]; idx = byte a56e(d[4]); rec = game[0xBC + 4*idx];
// a526(rec, d[0x18], d[0x1C]) zero runs a5a4(rec, d[0x18], d[0x1C]) and, if that is nonzero, cb80(result, game). The return value is
// findUnit(game, unit[0x94], unit[0x98]).
#include "d2game_e1570.h"
#include <cstring>
namespace {
uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
}
uint32_t d2game_e1570_port(uint32_t game, uint32_t unit, const D2GameE1570Env* e) {
    if (!unit) e->fatal(0x78b);
    const uint32_t d = rd(unit, 0x14);
    const uint32_t idx = (uint8_t)e->a56e(*(const uint8_t*)(uintptr_t)(d + 4));
    const uint32_t p = rd(d, 0x18), q = rd(d, 0x1c);
    const uint32_t rec = rd(game, 0xbc + idx * 4);
    if (!e->a526(rec, p, q)) {
        const uint32_t r = e->a5a4(rec, p, q);
        if (r) e->cb80(r, game);
    }
    return e->findUnit(game, rd(unit, 0x94), rd(unit, 0x98));
}
