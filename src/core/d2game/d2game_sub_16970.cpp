// src/core/d2game/d2game_sub_16970.cpp -- 1.13c D2Game.dll sub_16970 (RE-DONE, alpha lane), from the cached decompile.
// Unmapped callees are seams in retail argument order: 0xB07B0 check (register args), 0xF2540 move, 0x217D0 command lookup.
#include <cstdint>
#include <cstring>
typedef uint8_t* P;
static inline uint32_t r32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void w32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline P rp(const void* p, uint32_t o) { P v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
struct D2Sub16970Env {
    int32_t (*check_B07B0)(void* ctx);
    void    (*move_F2540)(void* ctx, void* unit, uint32_t, uint32_t, uint32_t x, uint8_t y, uint32_t);
    P       (*cmd_217D0)(void* ctx);
    void*   ctx;
};
// 1.13c D2Game+0x16970
uint32_t d2game_sub_16970(D2Sub16970Env& e, void* game, P unit, uint32_t, P arg) {
    if (!unit) return 0;
    if (!arg || !r32(arg, 4) || !r32(arg, 8)) return 0;
    if (!e.check_B07B0(e.ctx)) return 0;
    P sd = (r32(unit, 0) == 1 && r32(unit, 0x14)) ? rp(rp(unit, 0x14), 0x2C) : nullptr;
    w32(sd, 0x18, 7);
    e.move_F2540(e.ctx, unit, 0, 2, r32(arg, 4), (uint8_t)r32(arg, 8), 0);
    P c = e.cmd_217D0(e.ctx);
    if (c) { w32(c, 0xC, r32(arg, 4)); w32(c, 0x10, r32(arg, 8)); w32(c, 0x14, 0xC); w32(c, 0x18, 10); }
    return (uint32_t)(uintptr_t)c & 0xFFFFFF00u | 1;
}
