// src/core/d2game/d2game_17bc0.cpp -- 1.13c D2Game.dll sub_17BC0 (minion spawner tick) written from the disassembly.
// ECX = pGame, EDX = pUnit, stack S0 = pAiTickParam (ret 4). tick+0 = aiControl, +8 = target, +0x14 = distance, +0x1C = the monstats record;
// the record's int16 arrays at +0x56/+0x5C/+0x62/+0x68 are indexed by the game difficulty byte (game+0x6D). The EAX of the 0xF2380 call keeps the
// target's upper bytes (MOV AL, [rec+0x180]); the probe's target blob is 256-aligned so the log tags it.
#include <cstdint>
#include <cstring>

struct D2G17BC0Env {
    void (*idle)(uint32_t game, uint32_t unit, uint32_t mode);                          // 0xF22C0: EAX game, ESI unit, stack mode
    void (*count)(uint32_t* out, uint32_t game, uint32_t unit);                         // 0xB17B0: EAX 1, ECX &count, EDX game, stack (unit, 0x6FC35890)
    void (*act)(uint32_t unit, uint32_t eax, uint32_t skill, uint32_t target);          // 0xF2380: ECX unit, AL mode, stack (skill, target, 0, 0)
};
static inline uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }
static inline void wr(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)p + o, &v, 4); }
static inline int32_t s16(uint32_t p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 2); return v; }

// 1.13c D2Game+0x17BC0
extern "C" void d2game_17bc0_port(uint32_t game, uint32_t unit, uint32_t tick, const D2G17BC0Env* e)
{
    const uint32_t ai = rd(tick, 0), tgt = rd(tick, 8), ms = rd(tick, 0x1C);
    const uint32_t d = *(const uint8_t*)(uintptr_t)(game + 0x6D);
    if ((int32_t)rd(ai, 0x18) >= s16(ms, 0x56 + d * 2)) return;
    if ((int32_t)rd(tick, 0x14) > s16(ms, 0x68 + d * 2) || (int32_t)rd(game, 0xA8) < (int32_t)rd(ai, 0x14)) {
        e->idle(game, unit, (uint32_t)s16(ms, 0x5C + d * 2));
        return;
    }
    uint32_t n = 0;
    e->count(&n, game, unit);
    if (s16(ms, 0x170) >= 0 && (int32_t)n < s16(ms, 0x5C + d * 2)) {
        wr(ai, 0x18, rd(ai, 0x18) + 1);
        wr(ai, 0x14, (uint32_t)s16(ms, 0x62 + d * 2) + rd(game, 0xA8));
        e->act(unit, (tgt & 0xFFFFFF00u) | *(const uint8_t*)(uintptr_t)(ms + 0x180), (uint32_t)s16(ms, 0x170), tgt);
        return;
    }
    e->idle(game, unit, (uint32_t)s16(ms, 0x5C + d * 2));
}
