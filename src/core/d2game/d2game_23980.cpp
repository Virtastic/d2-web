// 1.13c D2Game+0x23980 -- weighted pick of a non-champion unique-mod row (EAX = game, stack (unit, flags), ret 8).
// game byte +0x6D (difficulty, < 3) selects the u16 weight at row +0x14+2*diff of the 0x20-byte MonUMod rows at
// *(sgptDataTables+0xC50) (count +0xC54); rows with a positive weight, +8 clear, 0x23100(unit, row, game[0x70])
// non-zero and flags[i] clear are collected (at most 256). A power-of-two total masks, any other modulos, the
// next 64-bit LCG state (seed*0x6AC690C5 + hi, stored at unit+0x20/+0x24). The walk subtracts weights.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game23980Env {
    uint32_t (*sub23100)(uint32_t unit, uint32_t entry, uint32_t force);
    uint32_t dataTables;   // *sgptDataTables
};
static inline uint32_t g23980_u32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
static inline int16_t g23980_s16(uint32_t a, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 2); return v; }
static inline uint8_t g23980_u8(uint32_t a, uint32_t o) { return *((const uint8_t*)(uintptr_t)a + o); }
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_game_23980(uint32_t game, uint32_t unit, uint32_t flags, const D2Game23980Env* e)
{
    if (game == 0 || g23980_u8(game, 0x6D) > 2) return 0;
    const uint32_t force = g23980_u32(game, 0x70);
    const int32_t count = (int32_t)g23980_u32(e->dataTables, 0xC54);
    uint32_t row = count > 0 ? g23980_u32(e->dataTables, 0xC50) : 0;
    const uint32_t slot = (uint32_t)g23980_u8(game, 0x6D) * 2 + 0x14;
    int32_t pick[512];
    int32_t n = 0;
    uint32_t total = 0;
    for (int32_t i = 0; i < count; ++i, row += 0x20) {
        if (g23980_s16(row, slot) <= 0 || g23980_u8(row, 8) != 0) continue;
        if (e->sub23100(unit, row, force) == 0 || g23980_u8(flags, (uint32_t)i) != 0) continue;
        const int32_t w = g23980_s16(row, slot);
        pick[2 * n] = i; pick[2 * n + 1] = w; total += (uint32_t)w;
        if (++n >= 256) break;
    }
    uint32_t roll;
    if ((int32_t)total < 1) roll = 0;
    else {
        const uint64_t v = (uint64_t)g23980_u32(unit, 0x20) * 0x6AC690C5ull + g23980_u32(unit, 0x24);
        std::memcpy((uint8_t*)(uintptr_t)unit + 0x20, &v, 8);
        roll = (total & (total - 1)) == 0 ? (total - 1) & (uint32_t)v : (uint32_t)(v & 0xFFFFFFFFull) % total;
    }
    for (int32_t k = 0; k < n; ++k) {
        if ((int32_t)roll < pick[2 * k + 1]) return pick[2 * k];
        roll -= (uint32_t)pick[2 * k + 1];
    }
    return 0;
}
