// 1.13c D2Game+0xE57D0: ECX = game, EDX = quest node (data +0x18). Once: broadcast, find the unit
// by id, and spawn up to eight effects around it from the ring table at D2Game+0x1028D0. Returns 1.
#include <cstdlib>
#include "d2game_h16.h"
namespace h16 {
static const int32_t kRing[8][2] = { {-3, -3}, {-3, 0}, {-3, 3}, {0, -3}, {0, 3}, {3, -3}, {3, 0}, {3, 3} };   // D2Game+0x1028D0
uint32_t e57d0(const Env& e, uint32_t game, uint32_t node)
{
    uint32_t d = rd32(node, 0x18);
    if (rd8(d, 0xc) != 0) return 1;
    e.sdfcc0(game, 0xE30C0);
    wr8(d, 0xc, 1); wr8(d, 0xd, 0);
    uint32_t id = rd32(d, 0x10);
    if (!game) abort();                                  // retail asserts and _exit(-1): not captured
    uint32_t u = rd32(game, 0x1320 + (id & 0x7f) * 4);
    if (!u) return 1;
    while (rd32(u, 0xc) != id) { u = rd32(u, 0xe4); if (!u) return 1; }
    uint32_t list = rd32(game, 0x10f4);
    if (!list) abort();
    uint64_t s = (uint64_t)rd32(list, 0x18) * 0x6ac690c5u + rd32(list, 0x1c);
    wr32(list, 0x18, (uint32_t)s); wr32(list, 0x1c, (uint32_t)(s >> 32));
    uint32_t h = e.o10331(u);
    uint32_t pos[2] = { 0, 0 };
    e.o10141(u, pos);
    for (uint32_t i = (uint32_t)s & 1; i < 8; ++i) {
        uint32_t pair[2] = { pos[0] + (uint32_t)kRing[i][0], pos[1] + (uint32_t)kRing[i][1] };
        uint32_t out = 0;
        e.s34130(h, pair, 3, 0x3f11, &out, 0x64);
        if (out) e.se1d90(pair[0], pair[1], game, out, 1, 0, 0);
    }
    return 1;
}
}
