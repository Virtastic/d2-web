// D2Client+0x62AF0: per-frame near-room refresh (called from 44A80). Callees via D2cHost (ordinals by number).
// g: +0 cntC204, +4 playerBBFC, +8 lastX 346C, +12 lastY 3468, +16 outX 3464, +20 outY 3460.
// Return: 1 = fatal path (retail calls _exit(-1) after Ordinal_10265/10024), else 0.
// Assumption: Ordinal_10383(room,&count) returns the room list (decompile lost it).
#include <emscripten.h>
#include <cstdint>
#include "client_host.h"
static uint32_t cl(D2cHost h, uint32_t rva, uint32_t a = 0, uint32_t b = 0) { D2cCall c = { rva, 0, a, b, 0, 0, { 0, 0, 0, 0 } }; return h(&c); }
// 1.13c D2Client+0x62AF0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_62af0(D2cHost h, uint32_t* g) {
    if (g[0] != 0) { g[0]--; return 0; }
    uint32_t pl = g[4 / 4];
    if (!pl) { cl(h, 10265); cl(h, 10024); return 1; }
    g[4] = cl(h, 10651);
    g[5] = cl(h, 11142, pl);
    int32_t dx = (int32_t)(g[2] - g[4]), dy = (int32_t)(g[3] - g[5]);
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    int32_t d = dy < dx ? dy + dx * 2 : dx + dy * 2;
    if (d / 2 > 0x4f) {
        g[2] = g[4]; g[3] = g[5];
        uint32_t room = cl(h, 10331, pl);
        if (room) {
            D2cCall c = { 10383, 0, room, 0, 0, 0, { 0, 0, 0, 0 } };
            uint32_t* list = (uint32_t*)(uintptr_t)h(&c);
            uint32_t n = c.out[0];
            cl(h, 10826, room);
            uint32_t lvl = *(uint32_t*)(uintptr_t)(cl(h, 10749) + 8);
            cl(h, 0x62710);
            for (uint32_t i = 0; i < n; i++) {
                cl(h, 10826, list[i]);
                if (lvl == *(uint32_t*)(uintptr_t)(cl(h, 10749) + 8)) cl(h, 0x62580, list[i]);
            }
        }
    }
    return 0;
}
