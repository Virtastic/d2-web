// 1.13c D2Game+0x23AE0 -- weighted pick of a champion unique-mod row (EAX = game, stack (unit), ret 4).
// Same table walk as 0x23980 over the u16 weight at row +0xE+2*diff, but only rows with +8 set, no flags array,
// and the roll comes from 0x1180(EAX = total weight, ECX = unit+0x20) even when nothing was collected. Nothing
// happens (answer 0) when the table is empty or null.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game23AE0Env {
    uint32_t (*sub23100)(uint32_t unit, uint32_t entry, uint32_t force);
    uint32_t (*sub1180)(uint32_t total, uint32_t seed);
    uint32_t dataTables;   // *sgptDataTables
};
static inline uint32_t g23ae0_u32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
static inline int16_t g23ae0_s16(uint32_t a, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 2); return v; }
static inline uint8_t g23ae0_u8(uint32_t a, uint32_t o) { return *((const uint8_t*)(uintptr_t)a + o); }
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_game_23AE0(uint32_t game, uint32_t unit, const D2Game23AE0Env* e)
{
    if (game == 0 || g23ae0_u8(game, 0x6D) > 2) return 0;
    const uint32_t force = g23ae0_u32(game, 0x70);
    const int32_t count = (int32_t)g23ae0_u32(e->dataTables, 0xC54);
    uint32_t row = g23ae0_u32(e->dataTables, 0xC50);
    if (count <= 0 || row == 0) return 0;
    const uint32_t slot = (uint32_t)g23ae0_u8(game, 0x6D) * 2 + 0xE;
    int32_t pick[512];
    int32_t n = 0;
    uint32_t total = 0;
    for (int32_t i = 0; i < count; ++i, row += 0x20) {
        if (g23ae0_s16(row, slot) <= 0 || g23ae0_u8(row, 8) == 0) continue;
        if (e->sub23100(unit, row, force) == 0) continue;
        const int32_t w = g23ae0_s16(row, slot);
        pick[2 * n] = i; pick[2 * n + 1] = w; total += (uint32_t)w;
        if (++n >= 256) break;
    }
    int32_t roll = (int32_t)e->sub1180(total, unit + 0x20);
    for (int32_t k = 0; k < n; ++k) {
        if (roll < pick[2 * k + 1]) return pick[2 * k];
        roll -= pick[2 * k + 1];
    }
    return 0;
}
