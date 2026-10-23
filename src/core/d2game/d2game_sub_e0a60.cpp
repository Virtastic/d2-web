#include <emscripten.h>
#include <cstdint>
// Same-owner/same-team test of two units; ECX = unit a, EAX = unit b, stack = game.
struct D2GameE0A60Env {
    uint32_t (*subE03A0)(uint32_t game, uint32_t id);   // 0xE03A0
    uint32_t (*sub0xF02A0)(uint32_t a, uint32_t b);     // 0xF02A0 (register args: a, b)
    uint32_t (*ordinal10666)(uint32_t a, uint32_t b);
};
static uint32_t rd(uint32_t p, uint32_t o) { return *(const uint32_t*)(uintptr_t)(p + o); }

// 1.13c D2Game+0xe0a60
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_E0A60(uint32_t a, uint32_t b, uint32_t game, const D2GameE0A60Env* e)
{
    if (a) {
        uint32_t g = game, id = 0;
        bool ok = false;
        if (rd(a, 0) == 1) {
            uint32_t r = rd(a, 0x14), s = r ? rd(r, 0x28) : 0;
            if (s && rd(s, 0x28)) { g = rd(s, 0x28); id = rd(s, 0x2c); ok = true; }
        } else if (rd(a, 0) == 3 && ((rd(a, 200) >> 10) & 1)) { id = rd(a, 0x98); ok = true; }
        if (ok) { uint32_t o = e->subE03A0(g, id); if (o) a = o; }
    }
    uint32_t p = b;
    while (p && rd(p, 0) == 1) {
        uint32_t r = rd(p, 0x14), s = r ? rd(r, 0x28) : 0;
        if (!s || !rd(s, 0x28)) break;
        uint32_t n = e->subE03A0(rd(s, 0x28), rd(s, 0x2c));
        if (!n || n == p) break;
        b = p = n;
    }
    // decompile loop leaves piVar1 = last probed value (in_EAX before update)
    if (a == p) return 0;
    if (a && rd(a, 0) == 0 && p && rd(p, 0) == 0) return e->sub0xF02A0(a, p);
    return e->ordinal10666(a, p) == 0;
}
