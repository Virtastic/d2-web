// 1.13c D2Game+0x23730 -- bind a unit to a game record and announce it (ECX = unit, EDX = game, one ignored stack
// word, ret 4). 0x216A0(EAX = unit) runs first. A type-1 unit's monster data (+0x14 -> +0x28 record) gets +0x2C = -1,
// +0x30 = 1, +0x28 = game. A unit with a non-zero mode (+0x10) then clears its action (0xA496(unit, 0)) and sends the
// state block (dword +4 = unit, byte +0x15 = 0x64) through 0x75E10(game, block, 1). A null unit does nothing else.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game23730Env {
    void (*sub216A0)(uint32_t unit);
    void (*ordinalA496)(uint32_t unit, uint32_t zero);
    void (*sub75E10)(uint32_t game, const uint32_t* block, uint32_t one);
};
static inline uint32_t g23730_u32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
static inline void g23730_w32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)a + o, &v, 4); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_23730(uint32_t unit, uint32_t game, const D2Game23730Env* e)
{
    e->sub216A0(unit);
    if (unit == 0) return;
    if (g23730_u32(unit, 0) == 1) {
        const uint32_t pd = g23730_u32(unit, 0x14);
        const uint32_t rec = pd ? g23730_u32(pd, 0x28) : 0;   // rec == 0 faults in retail (store to 0x2C)
        g23730_w32(rec, 0x2C, 0xFFFFFFFFu); g23730_w32(rec, 0x30, 1); g23730_w32(rec, 0x28, game);
    }
    if (g23730_u32(unit, 0x10) == 0) return;
    e->ordinalA496(unit, 0);
    uint32_t blk[8] = {}; blk[1] = unit; ((uint8_t*)blk)[0x15] = 0x64;
    e->sub75E10(game, blk, 1);
}
