// 1.13c D2Client+0xBE920: load the 13 cursor table rows (stride 12 at +0xD06D8) through 0x3CA80, then once more with id 0x20A when the mouse has a wheel.
// D2gfx #10048 (thunk 0xD0B4) must be nonzero or the Fog fatal path runs; GetSystemMetrics(0x4B) is the import at slot 0xCF0E8.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
static void fatal(D2cHost h) {
    D2cCall c = {}; c.rva = 0xBF92; c.s[0] = 0x179; uint32_t t = h(&c);
    c = {}; c.rva = 0xBF80; c.s[0] = 0x6fb8359f; c.s[1] = t; h(&c);
    c = {}; c.rva = 0x3664; c.s[0] = 0xffffffffu; h(&c);
}
static uint32_t gfx(D2cHost h) { D2cCall c = {}; c.rva = 0xD0B4; return h(&c); }
static uint32_t wheel(D2cHost h) { D2cCall c = {}; c.rva = 0xCF0E8; c.s[0] = 0x4b; return h(&c); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_be920(D2cHost h, const uint32_t* tb, uint32_t g3cc, uint32_t g898, uint32_t g890, uint32_t g88c, uint32_t d077c) {
    uint32_t g = gfx(h);
    if (!g) fatal(h);
    for (uint32_t i = 0; i < 13; i++) {
        D2cCall c = {}; c.rva = 0x3CA80; c.eax = g; c.ecx = tb[i * 3 + 1]; c.edx = tb[i * 3 + 2]; c.esi = tb[i * 3]; h(&c);
    }
    bool go = false;
    if (wheel(h) && g3cc) {
        if (g898 == 1) go = g890 != 0;
        else if (g898 == 2) go = g88c == 4 || g88c == 5;
    }
    if (!go && wheel(h) && g3cc && g898 == 1 && g890 == 0) go = true;
    if (!go) return;
    g = gfx(h);
    if (!g) fatal(h);
    D2cCall c = {}; c.rva = 0x3CA80; c.eax = g; c.ecx = 0x20a; c.edx = d077c; c.esi = 0; h(&c);
}
