// 1.13c D2Game+0x5C2A0 -- ACT5Q2_SpawnSoldierGroup(q, p2, p3), stdcall (ret 0xC). Up to 25 attempts at spawning a unit
// through 0xEEF10 from a request built off the quest record and the event; five successes end it. The caller's
// stack word at request +0x24 keeps only its low 16 bits (residue above).
#include <cstring>
#include "d2game_5c2a0.h"
namespace {
uint32_t rd(uint32_t b, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(b + o), 4); return v; }
void wr(uint32_t b, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(b + o), &v, 4); }
}  // namespace
uint32_t d2game_port_5c2a0(uint32_t q, uint32_t p2, uint32_t p3, const D2Game5C2A0Env* env)
{
    uint32_t n = 0;
    if (!q || !*(const uint8_t*)(uintptr_t)(q + 9)) return 0;
    for (uint32_t k = 0; k < 0x19; k++) {
        const uint32_t req[10] = { rd(q, 4), rd(p2, 8), 0, 0x216, 1, 0, rd(p2, 0x14), rd(p2, 0x18), 5, 0 };
        const uint32_t u = env->spawn(req);
        if (!u) continue;
        wr(u, 0xc4, rd(u, 0xc4) | 0x3000000);
        wr(rd(q, 0x18), 0xb0 + n * 4 + p3 * 0x14, rd(u, 0xc));
        n++;
        const uint32_t game = rd(q, 4), list = rd(game, 0x10f4);
        if (list)
            for (uint32_t node = rd(list, 0); node; node = rd(node, 0xf4))
                if (rd(node, 0) == 0x20) { env->arm(u, node, game); break; }
        if (n > 4) return n;
    }
    return n;
}
