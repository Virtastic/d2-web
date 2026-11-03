// src/core/d2game/d2game_16610.cpp -- 1.13c D2Game.dll sub_16610 written from the disassembly.
// EAX = pUnit, stack S0 = pGame, S1 = unused, S2 = distance (ret 0xC). A monster whose AI control list (aiControl+0x20) holds a type-10 node
// is sent home: farther than S2 from (x, y) it queues a type-4 command (x, y, 0xC, 10) and idles in mode 10; otherwise 0xB07B0's value with AL cleared.
// No such node: tail-call 0x15D10. The result is the full EAX (AL=1 keeps F22C0's upper bytes).
#include <cstdint>
#include <cstring>

struct D2G16610Env {
    uint32_t (*dist)(uint32_t unit, uint32_t x, uint32_t y);       // 0xB07B0: EAX unit, stack (x, y), ret 8
    uint32_t (*cmd)(uint32_t game);                                // 0x217D0: EAX 4, EBX 0, stack game; the command record or 0
    uint32_t (*idle)(uint32_t game, uint32_t unit, uint32_t mode); // 0xF22C0: EAX game, ESI unit, stack mode
    uint32_t (*home)(uint32_t unit, uint32_t game);                // 0x15D10: EAX unit, stack game
};
static inline uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }
static inline void wr(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)p + o, &v, 4); }

// 1.13c D2Game+0x16610
extern "C" uint32_t d2game_16610_port(uint32_t unit, uint32_t game, uint32_t, uint32_t maxDist, const D2G16610Env* e)
{
    uint32_t n = 0;
    if (unit && rd(unit, 0) == 1) {
        const uint32_t ai = rd(unit, 0x14), ctl = ai ? rd(ai, 0x28) : 0, lst = ctl ? rd(ctl, 0x20) : 0;
        if (lst) {
            n = rd(lst, 0);
            while (rd(n, 8) != 10 && n != lst) n = rd(n, 0);
            if (rd(n, 8) != 10) n = 0;
        }
    }
    if (!n) return e->home(unit, game);
    const uint32_t x = rd(n, 0xC), y = rd(n, 0x10);
    const uint32_t d = e->dist(unit, x, y);
    if (d <= maxDist) return d & 0xFFFFFF00u;
    const uint32_t c = e->cmd(game);
    if (c) { wr(c, 0xC, rd(n, 0xC)); wr(c, 0x10, rd(n, 0x10)); wr(c, 0x14, 0xC); wr(c, 0x18, 10); }
    return (e->idle(game, unit, 10) & 0xFFFFFF00u) | 1;
}
