// 1.13c D2Game+0x94970: scatter a skill's effect around the caster. Fastcall ECX = game, EDX = unit, stack = skill id, parameter word (ret 8).
// Steps the unit's seed (unit+0x20) once, takes the skill row (sgptDataTables +0xB98, stride 0x23C, count +0xBA0), reads the unit's
// position (0x9FA30) and the skill level (0x91340, must be > 0), the radius (0x91240, else the row word +0x64 through thunk 0x2AC9A), then
// radius^2 times draws a point in the square of side 2*radius; points inside the circle that 0x21280 accepts are handed to 0x8F930 in a
// 23-dword block {1, unit, 0, 0, level, x, y, ..., skill (+0x2C), parameter (+0x30)}. Returns 1, or 0 when any gate fails.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_94970.h"

namespace {
inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 4); return v; }
inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)(a + o), &v, 4); }
inline uint32_t roll(uint32_t* s, int32_t range) {   // 0x1180: s[0] = low, s[1] = high; range <= 0 gives 0 and leaves the seed
    if (range < 1) return 0;
    const uint64_t v = (uint64_t)s[0] * 0x6AC690C5ull + s[1];
    s[0] = (uint32_t)v; s[1] = (uint32_t)(v >> 32);
    return (uint32_t)v % (uint32_t)range;
}
}

// 1.13c D2Game+0x94970
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_94970_port(const G94970Env* e, uint32_t game, uint32_t unit, int32_t skill, uint32_t p4) {
    const uint64_t s = (uint64_t)rd32(unit, 0x20) * 0x6AC690C5ull + rd32(unit, 0x24);
    wr32(unit, 0x20, (uint32_t)s); wr32(unit, 0x24, (uint32_t)(s >> 32));
    if (skill < 0) return 0;
    const uint32_t dt = rd32(e->sgptCell, 0);
    if (skill >= (int32_t)rd32(dt, 0xba0)) return 0;
    const uint32_t row = (uint32_t)skill * 0x23c + rd32(dt, 0xb98);
    if (row == 0) return 0;
    int32_t px = 0, py = 0;
    if (e->f9fa30(unit, &px, &py) == 0) return 0;
    const int32_t level = e->f1340((uint32_t)skill, unit);
    if (level <= 0) return 0;
    int32_t radius = e->f1240((uint32_t)skill, unit, p4);
    if (radius == 0) radius = e->t2ac9a(unit, rd32(row, 0x64), (uint32_t)skill, p4);
    uint32_t blk[0x17] = {};
    blk[0] = 1; blk[1] = unit; blk[4] = (uint32_t)level; blk[11] = (uint32_t)skill; blk[12] = p4;
    uint32_t seed[2] = {};
    e->t2a628(seed, (uint32_t)s);
    const uint32_t room = e->t2a478(unit);
    const int32_t sq = radius * radius;
    for (int32_t n = sq; n > 0; n--) {
        const int32_t dx = radius - (int32_t)roll(seed, 2 * radius);
        const int32_t dy = radius - (int32_t)roll(seed, 2 * radius);
        if (dy * dy + dx * dx > sq) continue;
        blk[5] = (uint32_t)(dx + px); blk[6] = (uint32_t)(dy + py);
        if (e->f21280(room, dx + px, dy + py) != 0) e->f8f930(game, blk);
    }
    return 1;
}
