// src/core/d2game/d2game_15f90.cpp -- 1.13c D2Game.dll sub_15F90 written from the disassembly.
// EAX = pAiTickParam, ECX = pGame, stack S0 = pUnit (ret 4). Walks the AI control's path-node list: for the unit's class skill it issues
// 0xF2380 (ECX unit, AL mode 4, stack skill, 0, node x, node y) and steps the node index; otherwise a frame-gap reset after 700 frames.
#include <cstdint>
#include <cstring>

struct D2G15F90Env {
    void (*act)(uint32_t unit, uint32_t eax, uint32_t skill, uint32_t x, uint32_t y);   // 0xF2380: ECX unit, EAX (AL=4), stack (skill, 0, x, y)
};
static inline uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }
static inline void wr(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)p + o, &v, 4); }
static inline int32_t rd16s(uint32_t p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 2); return v; }

// 1.13c D2Game+0x15F90.  dt = the sgptDataTables target (monstats count +0xA80, records +0xA78 of 0x1A8, skills count +0xBA0).
extern "C" uint32_t d2game_15f90_port(uint32_t tick, uint32_t game, uint32_t unit, uint32_t dt, const D2G15F90Env* e)
{
    const uint32_t ai = rd(tick, 0), map = rd(ai, 0x38);
    if (map && (int32_t)rd(ai, 0x14) < (int32_t)rd(map, 0)) {
        if (!unit) return 0;
        const int32_t cls = (int32_t)rd(unit, 4);
        if (cls < 0 || cls >= (int32_t)rd(dt, 0xA80)) return 0;
        const uint32_t rec = (uint32_t)cls * 0x1A8 + rd(dt, 0xA78);
        if (!rec) return 0;
        const int32_t sk = rd16s(rec, 0x170);
        if (sk < 0 || sk >= (int32_t)rd(dt, 0xBA0)) return 0;
        const uint32_t node = rd(map, 4) + rd(ai, 0x14) * 12;
        const uint32_t x = rd(node, 4), y = rd(node, 8);
        e->act(unit, (x & 0xFFFFFF00u) | 4, (uint32_t)sk, x, y);
        wr(ai, 0x14, rd(ai, 0x14) + 1);
        wr(ai, 0x18, rd(game, 0xA8));
        return 1;
    }
    int32_t d = (int32_t)(rd(game, 0xA8) - rd(ai, 0x18));
    if (d < 0) d = -d;
    if (d > 700) wr(ai, 0x14, 0);
    return 0;
}
