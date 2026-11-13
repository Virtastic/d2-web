// 1.13c D2Game+0x76270: unit-removal event handler (a table callback). ECX = game, EDX = event record (+4 the unit, +0x14 a byte).
// It lets 0xDA750/0x75350/0x75900 clean the unit up, counts a removal in the game's per-slot record (+0x2D0), then by the
// unit's class row (+0xE flags & mask, +2 the sub class): sub class 0xD4 spawns a unit through 0xA28F0 and a timed effect through 0xDE380,
// 0x1B9 goes to 0x760F0, anything else to 0x755C0. Always returns 1.
// Delta against the decompile: Ghidra drops the dead 0xA478 push (it is 0xB072's second argument), cuts 0xA42A to one argument (it takes three and
// leaves two words for 0xAD7E) and shows 0xDE380 with five.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_76270.h"

namespace {
inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 4); return v; }
inline uint32_t rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
inline int32_t rd16s(uint32_t a, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 2); return v; }
inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)(a + o), &v, 4); }
}

// 1.13c D2Game+0x76270
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_76270_port(const G76270Env* e, uint32_t game, uint32_t rec) {
    const uint32_t unit = rd32(rec, 4);
    e->fDA750(unit, game);
    e->t2A4A2(unit, 0);
    e->f75350(unit, game, rd8(rec, 0x14));
    e->f75900(rec, game);
    bool skip = false;
    uint32_t idx = 0;
    if (unit != 0 && rd32(unit, 0) == 1) {
        const uint32_t pp = rd32(unit, 0x14);
        if (pp != 0 && (rd8(pp, 0x5c) & 2) != 0) skip = true;
        else if (pp != 0) idx = rd32(pp, 0x58);
    }
    if (!skip) {
        const uint32_t slot = rd32(game, 0xf0 + idx * 4);
        if (e->t2A514(unit) == 0) wr32(slot, 0x2d0, rd32(slot, 0x2d0) + 1);
    }
    const uint32_t id = unit == 0 ? 0xFFFFFFFFu : rd32(unit, 0xc);
    e->t2B072(e->t2A478(unit), id);
    if (unit == 0) return 1;
    const int32_t cls = (int32_t)rd32(unit, 4);
    if (cls < 0) return 1;
    const uint32_t dt = e->dataTables();
    const int32_t cnt = (int32_t)rd32(dt, 0xa80);
    if (cls >= cnt) return 1;
    const uint32_t row = (uint32_t)cls * 0x1a8 + rd32(dt, 0xa78);
    if (row == 0) return 1;
    if ((rd8(row, 0xe) & e->bitMasks()) == 0) return 1;
    if (rd32(unit, 0) == 1) {
        const int32_t sub = rd16s(row, 2);
        if (sub >= 0 && sub < cnt) {
            if (sub == 0xd4) {
                const uint32_t p1 = rd32(unit, 0x2c);
                const uint32_t v1 = p1 != 0 ? e->t2A454(p1) : 0;
                const uint32_t v2 = p1 != 0 ? e->t2A3B8(p1) : 0;
                const uint32_t a[8] = { unit, 0, 1, 0, 0, v1, v2, 1 };
                const uint32_t mis = e->fA28F0(game, 0x75, a);
                if (mis == 0) return 1;
                const uint32_t ent = e->t2A7BA(rd8(game, 0x6d));
                if (ent == 0) return 1;
                uint32_t s1[14] = {};
                const uint32_t r = e->t2A42A(unit, 0xc, 0);
                e->t2AD7E(rd32(unit, 4), rd32(game, 0x74), rd8(game, 0x6d), r, 1, s1);
                const int32_t esi = e->f14D0((int32_t)rd32(ent, 0x3c), 100, (int32_t)s1[1]);
                int32_t ebp;
                if (esi <= 0x100000) ebp = (int32_t)((uint32_t)esi * 60u) / 100;
                else if ((esi & ~0xf) < 0x640) ebp = (int32_t)(((int64_t)esi * 60) / 100);
                else ebp = (esi / 100) * 60;
                uint32_t s2[28] = {};
                const uint32_t roll = e->f11D0(unit + 0x20, esi - ebp);
                s2[2] = (roll + (uint32_t)ebp) << 7;
                const uint32_t b[10] = { game, mis, v1, v2, 5, (uint32_t)(uintptr_t)s2, 0, 0, 0, 0x581 };
                e->fDE380(b);
                return 1;
            }
            if (sub == 0x1b9) { e->f760f0(unit, game); return 1; }
        }
    }
    e->f755c0(unit, game);
    return 1;
}
