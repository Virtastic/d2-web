// 1.13c D2Game+0xA4610 AITHINK_GetTargetForBoss: written from the disassembly. The 8 per-player
// target lists game+0x10F8[0..7] are culled by pfCull and scored by 0xA3E80; list 8 is the act walk, list 9 the town-portal fallback.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_a4610.h"

typedef uintptr_t A;
static inline uint32_t rd32(A p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint8_t rd8(A p, uint32_t o) { return ((const uint8_t*)p)[o]; }

// 1.13c D2Game+0xA4610
extern "C" EMSCRIPTEN_KEEPALIVE
A d2_game_A4610(A game, A unit, int32_t* a2, int32_t* a3, A aiCmd, A pfCull, const D2GameA4610Env* e)
{
    if (!pfCull) e->fail(0x37A);
    e->fA478(unit);
    A list = game + 0x10F8;
    A node = rd32(game, 0x10F8);
    int32_t nMax = 0, nCount = 0;
    A target = 0;
    const uint32_t type = rd32(unit, 0);
    const bool direct = type == 2 || (type > 3 && type < 6);
    int32_t x, y;
    if (direct) x = (int32_t)rd32(rd32(unit, 0x2C), 0xC);
    else if (!rd32(unit, 0x2C)) x = 0;
    else x = e->fA454(rd32(unit, 0x2C));
    if (direct) y = (int32_t)rd32(rd32(unit, 0x2C), 0x10);
    else if (!rd32(unit, 0x2C)) y = 0;
    else y = e->fA3B8(rd32(unit, 0x2C));
    const uint8_t act = rd8(unit, 0x18);
    for (int i = 0; i < 8; ++i) {
        if (node) {
            A cand = rd32(node, 0);
            if (!cand || rd32(cand, 0) != 0) e->fail(0x395);
            if (e->cull(unit, cand)) {
                const uint32_t anim = rd32(cand, 0x10);
                int32_t score = (anim == 0x11 || anim == 0) ? 0 : e->f3E80(aiCmd, cand, unit);
                for (;;) {
                    if (nMax < score) { nMax = score; target = cand; }
                    node = rd32(node, 8);
                    ++nCount;
                    if (!node) break;
                    cand = rd32(node, 0);
                    score = e->f3E80(aiCmd, cand, unit);
                }
            }
        }
        list += 4;
        node = rd32(list, 0);
    }
    // list 8: the act walk
    for (; node; node = rd32(node, 8)) {
        A cand = rd32(node, 0);
        if (act == rd8(cand, 0x18)) {
            int32_t score = e->f3E80(aiCmd, cand, unit);
            if (nMax < score) { nMax = score; target = cand; }
            ++nCount;
        }
    }
    // list 9: the best same-act candidate, taken when it stands within 5 of the unit and the unit cannot reach the current target
    node = rd32(list, 4);
    int32_t nTemp = 0;
    A pTemp = 0;
    for (; node; node = rd32(node, 8)) {
        A cand = rd32(node, 0);
        if (act == rd8(cand, 0x18)) {
            int32_t score = e->f3E80(aiCmd, cand, unit);
            if (nTemp < score) { nTemp = score; pTemp = cand; }
        }
    }
    if (pTemp && e->fB08F0(pTemp, x, y) < 5 && !e->fA754(unit, target, 0)) {
        A path = rd32(unit, 0x2C);
        e->fA5FE(path, target);
        e->fAAFC(path, 2);
        e->fAAF0(path, unit, 0);
        if (!e->fAC70(path)) { target = pTemp; nMax = nTemp; }
    }
    *a3 = nCount;
    *a2 = nMax;
    return target;
}
