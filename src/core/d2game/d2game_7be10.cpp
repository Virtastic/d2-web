// Monster AI reset step: EAX = game, ESI = unit. Clears the AI flag bits, re-anchors the unit at its own position twice
// (collision masks 0x1000 then 0x100), sets mode 5, clears the path target, fires each type-2 event node, then queues the
// AI command through 0x8EA80. Returns 0x8EA80's EAX.
#include <emscripten.h>
#include "d2game_7b860.h"
using namespace d2g7b860;

namespace {
bool isStatic(const uint8_t* u) { const int32_t t = (int32_t)rd(u, 0); return t == 2 || (t > 3 && t <= 5); }
int32_t posY(const uint8_t* u, const D2g7b860Env* e) { const uint8_t* p = ptr(u, 0x2c); return isStatic(u) ? (int32_t)rd(p, 0x10) : (p ? e->a3b8(p) : 0); }
int32_t posX(const uint8_t* u, const D2g7b860Env* e) { const uint8_t* p = ptr(u, 0x2c); return isStatic(u) ? (int32_t)rd(p, 0xc) : (p ? e->a454(p) : 0); }
}

// 1.13c D2Game+0x7BE10
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_7be10(const uint8_t* game, uint8_t* unit, const D2g7b860Env* e) {
    if (unit) wr(unit, 0xc4, rd(unit, 0xc4) & 0xfffffff1u);
    static const uint32_t kFlags[2] = { 0x1000u, 0x100u };
    for (uint32_t flag : kFlags) {
        const int32_t y = posY(unit, e);
        const int32_t x = posX(unit, e);
        e->afee(e->a478(unit, x, y, flag));
    }
    e->a6d6(unit, 5);
    e->ab08(ptr(unit, 0x2c), 0);
    for (const uint8_t* n = ptr(unit, 0xdc); n;) {
        const uint8_t* next = ptr(n, 0x24);
        if (n[0] == 2) e->c8e6f0(n, game);
        n = next;
    }
    return e->c8ea80(game, unit, 2, rd(game, 0xa8) + 0xc, 0, 0, 0);
}
