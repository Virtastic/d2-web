// 1.13c D2Game+0x88A70: AI think step with a stat-bonus state (fastcall: ECX = game, EDX = unit, stack = AI tick record,
// ret 4). Record: +0 control, +8 target, +0x14 range, +0x18 combat flag, +0x1C MonStats row; control +0x14 = bonus active,
// +0x18 = next-swing tick. Row words are indexed by game+0x6D (difficulty): +0x56 skill roll, +0x5C skill count limit,
// +0x62 command roll, +0x68 swing roll, +0x6E idle frames, +0x74 bonus id (2 selects state 0x5B, else 0x5A), +0x7A swing count
// limit, +0x80 swing cooldown; row +0x170 skill id, +0x180 skill mode byte. Every roll is the unit's seed (unit+0x20)
// stepped through 0x6AC690C5 and taken mod 100. Callees are seams on D2Game77dEnv with the register arguments of the disassembly.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_77d_env.h"

namespace {
inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 4); return v; }
inline int32_t rdS16(uint32_t a, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 2); return v; }
inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)(a + o), &v, 4); }
inline uint32_t roll100(uint32_t seed) {
    const uint64_t s = (uint64_t)rd32(seed, 0) * 0x6AC690C5ull + (uint64_t)rd32(seed, 4);
    wr32(seed, 0, (uint32_t)s); wr32(seed, 4, (uint32_t)(s >> 32));
    return (uint32_t)s % 100u;
}
}

// 1.13c D2Game+0x88A70
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_88a70_port(const D2Game77dEnv* e, uint32_t game, uint32_t unit, uint32_t task) {
    const uint32_t ctl = rd32(task, 0), target = rd32(task, 8), row = rd32(task, 0x1c), flag = rd32(task, 0x18);
    const int32_t range = (int32_t)rd32(task, 0x14);
    const uint32_t seed = unit + 0x20;
    const uint32_t d = *(const uint8_t*)(uintptr_t)(game + 0x6d) * 2u;
    auto bonus = [&](uint32_t on) {
        const int32_t id = rdS16(row, 0x74 + d);
        if (id != 0) e->th2A3CA(unit, id == 2 ? 0x5b : 0x5a, on);
    };
    auto idle = [&] { e->subF22C0(game, (uint32_t)rdS16(row, 0x6e + d)); };
    if (rd32(ctl, 0x14) != 0) bonus(0);
    if (flag != 0) {
        if ((int32_t)roll100(seed) >= rdS16(row, 0x62 + d)) { idle(); return; }
        e->ordinal11103(unit, 0);
        uint8_t cmd[0x20];
        std::memset(cmd, 0, sizeof cmd);
        const uint32_t kind = 5;
        std::memcpy(cmd, &kind, 4); std::memcpy(cmd + 4, &unit, 4);
        uint32_t bl = 0x64;
        if (unit != 0) {
            const uint32_t rec = e->sub1320(rd32(unit, 4));
            if (rec != 0 && (*(const uint8_t*)(uintptr_t)(rec + 0x104) & rd32(e->globalF8BE0(), 0x14)) != 0) bl = 0x65;
        }
        cmd[0x15] = (uint8_t)bl;
        std::memcpy(cmd + 8, &target, 4);
        e->sub75E10(game, cmd, 1);
        return;
    }
    if (range < rdS16(row, 0x7a + d)) {
        if ((int32_t)roll100(seed) < rdS16(row, 0x68 + d)) {
            if ((int32_t)rd32(ctl, 0x18) < (int32_t)rd32(game, 0xa8)) {
                e->subF2220(4, unit, game, target);
                wr32(ctl, 0x18, (uint32_t)rdS16(row, 0x80 + d) + rd32(game, 0xa8));
                return;
            }
            idle(); return;
        }
    }
    if (rdS16(row, 0x170) >= 0 && range < rdS16(row, 0x5c + d) && (int32_t)roll100(seed) < rdS16(row, 0x56 + d)) {
        const uint32_t y = (uint32_t)e->sub1130(target), x = (uint32_t)e->sub1100(target);
        const uint32_t skill = (uint32_t)rdS16(row, 0x170);
        if (e->subF3E30(y, target, game, skill, x) != 0) {
            e->subF2380(*(const uint8_t*)(uintptr_t)(row + 0x180), unit, game, skill, target, 0, 0);
            bonus(1);
            wr32(ctl, 0x14, 1);
            return;
        }
    }
    if (roll100(seed) < 0x32) { e->subF2540(game, unit, target, 2, 0, 0, 1, 7); return; }
    idle();
}
