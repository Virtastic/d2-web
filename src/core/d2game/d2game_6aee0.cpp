// D2Game+0x6AEE0 -- create a unit next to another unit from a filled-in creation block.
// ebx = unit, stack (game, classId, a3) ret 0xc; classId < 0 -> 0.
// #a544(unit, &c); room2 = 0xE930(esi &c; #a478(unit), &p, 1); room2 == 0 -> 0. A zeroed 0x84-byte block:
// +0 unit, +8 game, +0xc 0xE5D0(eax unit; 0), +0x14 classId, +0x18 3, +0x1c p.x, +0x20 p.y, +0x24 room2,
// +0x28 (u16)1, +0x2a (u16)game[0x78], +0x30 a3. Returns 0x11490(game, &block, 0).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game6AEE0Env {
    void     (*a544)(uint32_t unit, uint32_t coord);
    uint32_t (*a478)(uint32_t unit);
    uint32_t (*subE930)(uint32_t esi, uint32_t room, uint32_t pt, uint32_t one);
    uint32_t (*subE5D0)(uint32_t eax, uint32_t s1);
    uint32_t (*sub11490)(uint32_t game, uint32_t block, uint32_t zero);
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x6AEE0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_6aee0(uint32_t unit, uint32_t game, int32_t classId, uint32_t a3, const D2Game6AEE0Env* env)
{
    if (classId < 0) return 0;
    uint32_t c[2] = {}, p[2] = {};
    env->a544(unit, (uint32_t)(uintptr_t)c);
    const uint32_t room2 = env->subE930((uint32_t)(uintptr_t)c, env->a478(unit), (uint32_t)(uintptr_t)p, 1);
    if (room2 == 0) return 0;
    uint32_t b[0x21] = {};
    b[1] = 0;
    const uint16_t w78 = (uint16_t)rd32(game + 0x78);
    b[0x14 / 4] = (uint32_t)classId;
    b[0x24 / 4] = room2;
    b[0x1c / 4] = p[0];
    b[0] = unit;
    b[2] = game;
    b[0x18 / 4] = 3;
    b[0x20 / 4] = p[1];
    b[0x28 / 4] = 1u | ((uint32_t)w78 << 16);
    b[0x30 / 4] = a3;
    b[0xc / 4] = env->subE5D0(unit, 0);
    return env->sub11490(game, (uint32_t)(uintptr_t)b, 0);
}
