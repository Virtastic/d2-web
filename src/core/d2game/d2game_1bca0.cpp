// src/core/d2game/d2game_1bca0.cpp -- 1.13c D2Game.dll sub_1BCA0 (invisible spawner tick) written from the disassembly.
// ECX = pGame, EDX = pUnit, stack S0 = pAiTickParam (ret 4). tick+0 = aiControl, +0x14 = distance, +0x1C = monstats record (int16 arrays at
// +0x56 count, +0x5C range, +0x62 delay, indexed by game+0x6D). 1.13c picks the monster id with 0x...ADFC(room, 0x60 or -1) where the vendored
// 1.10f code hard-codes one class; the spawn itself is 0x47450 (find a spot) then 0xEF650 (create, flags 2, 0x42).
#include <cstdint>
#include <cstring>

struct D2G1BCA0Env {
    void (*mode)(uint32_t unit, uint32_t v);                                                     // import 0xA496: stack (unit, 0)
    void (*req)(uint32_t game, const uint32_t* st);                                              // 0x75E10: stack (game, &8-dword block, 1)
    uint32_t (*room)(uint32_t unit);                                                             // import 0xA478: stack unit
    uint32_t (*pick)(uint32_t room, uint32_t id);                                                // import 0xADFC: stack (room, id)
    uint32_t (*spot)(uint32_t game, uint32_t room, uint32_t id, uint32_t* o1, uint32_t* o2);     // 0x47450: stack (game, room, 0, id, &o1, &o2, 0)
    uint32_t (*make)(uint32_t id, uint32_t game, uint32_t room, uint32_t o1, uint32_t o2);       // 0xEF650: ECX id, EDX 1, stack (game, room, o1, o2, 2, 0x42)
    void (*idle)(uint32_t game, uint32_t unit, uint32_t mode);                                   // 0xF22C0: EAX game, ESI unit, stack mode
    int32_t dtCount;                                                                             // sgptDataTables target +0xA80
};
static inline uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }
static inline void wr(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)p + o, &v, 4); }
static inline int32_t s16(uint32_t p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 2); return v; }

// 1.13c D2Game+0x1BCA0
extern "C" void d2game_1bca0_port(uint32_t game, uint32_t unit, uint32_t tick, const D2G1BCA0Env* e)
{
    const uint32_t ai = rd(tick, 0), ms = rd(tick, 0x1C);
    const int32_t dist = (int32_t)rd(tick, 0x14), t14 = (int32_t)rd(ai, 0x14);
    const uint32_t d = *(const uint8_t*)(uintptr_t)(game + 0x6D);
    if (t14 == 0) wr(ai, 0x18, (uint32_t)s16(ms, 0x56 + d * 2));
    if (dist <= s16(ms, 0x5C + d * 2)) {
        if ((int32_t)rd(ai, 0x18) <= 0) {
            e->mode(unit, 0);
            const uint32_t st[8] = { 0, unit, 0, 0, 0, 0x6400, 0, 0 };
            e->req(game, st);
            return;
        }
        if (t14 <= (int32_t)rd(game, 0xA8)) {
            const uint32_t room = e->room(unit);
            const uint32_t id = e->pick(room, e->dtCount > 0x60 ? 0x60u : 0xFFFFFFFFu);
            uint32_t o1 = 0, o2 = 0;
            if (e->spot(game, room, id, &o1, &o2) && e->make(id, game, room, o1, o2)) {
                wr(ai, 0x18, rd(ai, 0x18) - 1);
                wr(ai, 0x14, (uint32_t)s16(ms, 0x62 + d * 2) + rd(game, 0xA8));
            }
        }
    }
    e->idle(game, unit, 0xF);
}
