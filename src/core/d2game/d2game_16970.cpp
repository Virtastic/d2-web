// src/core/d2game/d2game_16970.cpp -- 1.13c D2Game.dll sub_16970 (AI "move to point" command) written from the disassembly.
// ECX = pGame, EDX = pUnit, stack S0 unused, S1 = pArg (ret 8); pArg+4 / +8 are the x / y dwords. Returns AL (1 on success).
// A monster unit's AI control (unit+0x14)+0x2C gets state 7 (a non-monster unit crashes retail here: not reachable from a vector).
#include <cstdint>
#include <cstring>

struct D2G16970Env {
    uint32_t (*dist)(uint32_t unit, uint32_t x, uint32_t y);                          // 0xB07B0: EAX unit, stack (x, y), ret 8
    void (*move)(uint32_t game, uint32_t unit, uint32_t x, uint32_t y);               // 0xF2540: ECX game, EDX unit, stack (0, 2, x, y, 0, 0), ret 0x18
    uint32_t (*cmd)(uint32_t game);                                                   // 0x217D0: EAX 4, EBX 0, stack game; the command record or 0
};
static inline uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }
static inline void wr(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)p + o, &v, 4); }

// 1.13c D2Game+0x16970
extern "C" uint32_t d2game_16970_port(uint32_t game, uint32_t unit, uint32_t arg, const D2G16970Env* e)
{
    if (!unit) return 0;
    if (!arg || !rd(arg, 4) || !rd(arg, 8)) return 0;
    if (e->dist(unit, rd(arg, 4), rd(arg, 8)) == 0) return 0;
    const uint32_t ai = (rd(unit, 0) == 1 && rd(unit, 0x14)) ? rd(rd(unit, 0x14), 0x2C) : 0;
    wr(ai, 0x18, 7);
    e->move(game, unit, rd(arg, 4), rd(arg, 8));
    const uint32_t c = e->cmd(game);
    if (c) { wr(c, 0xC, rd(arg, 4)); wr(c, 0x10, rd(arg, 8)); wr(c, 0x14, 0xC); wr(c, 0x18, 10); }
    return 1;
}
