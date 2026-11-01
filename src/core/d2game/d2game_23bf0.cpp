// 1.13c D2Game+0x23BF0 -- hand a type-1 idle unit over to the unit its monster data links to (ECX = game, EDX = unit,
// two ignored stack words, ret 8). Needs mode 0, type 1, +0x14 -> +0x28 record with link +0x28; 0xE03A0(EAX = rec[0x30],
// EDX = rec[0x2C], ECX = rec[0x28]) resolves it. Then 0x21490(EAX = unit, EBX = callback 0x23730, game, 0) runs; a type-1
// result gets its monster-data record (+0x14 -> +0x28) set to -1/1/game at +0x2C/+0x30/+0x28; a result that is not the
// unit and is type 1 is announced with 0x75070(ESI = block, 0) filling the block, then 0x75E10(game, block, 1).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game23BF0Env {
    uint32_t (*subE03A0)(uint32_t a, uint32_t d, uint32_t c);
    void (*sub21490)(uint32_t unit, uint32_t callback, uint32_t game, uint32_t zero);
    void (*sub75070)(uint32_t* block, uint32_t zero);
    void (*sub75E10)(uint32_t game, const uint32_t* block, uint32_t one);
    uint32_t callback23730;
};
static inline uint32_t g23bf0_u32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
static inline void g23bf0_w32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)a + o, &v, 4); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_23BF0(uint32_t game, uint32_t unit, const D2Game23BF0Env* e)
{
    if (unit == 0 || g23bf0_u32(unit, 0x10) != 0 || g23bf0_u32(unit, 0) != 1) return;
    const uint32_t pd = g23bf0_u32(unit, 0x14);
    if (pd == 0) return;
    const uint32_t rec = g23bf0_u32(pd, 0x28);
    if (rec == 0 || g23bf0_u32(rec, 0x28) == 0) return;
    const uint32_t res = e->subE03A0(g23bf0_u32(rec, 0x30), g23bf0_u32(rec, 0x2C), g23bf0_u32(rec, 0x28));
    if (res == 0) return;
    e->sub21490(unit, e->callback23730, game, 0);
    if (g23bf0_u32(res, 0) == 1) {
        const uint32_t rp = g23bf0_u32(res, 0x14);
        const uint32_t r2 = rp ? g23bf0_u32(rp, 0x28) : 0;   // r2 == 0 faults in retail
        g23bf0_w32(r2, 0x2C, 0xFFFFFFFFu); g23bf0_w32(r2, 0x30, 1); g23bf0_w32(r2, 0x28, game);
    }
    if (res != unit && g23bf0_u32(res, 0) == 1) {
        uint32_t blk[8] = {};
        e->sub75070(blk, 0);
        e->sub75E10(game, blk, 1);
    }
}
