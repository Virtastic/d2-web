// src/core/d2client/click_rect_8ced0.cpp -- D2Client.dll 1.13c, base 0x6FAB0000.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"

// 1.13c D2Client+0x8CED0 (132 B, implicit ESI = event struct p; callers 0x9FF46/0x6A037A): mouse-event hit test.
// p+0xC u16 x, p+0xE u16 y; p+0x18/0x1C = handled/pass flags; host(511) = Storm#511 (eax = p) consumes the event.
// Hit box: x in [W-dx-0x12E, W-dx-0x10E], y in [dy-0x60+H, dy-0x40+H] with W=0x6FB8BC48, H=0x6FB8BC4C,
// dx=0x6FBCB9A0, dy=0x6FBCB9A4. On hit sets flag 0x6FBCBEDC=1 (*hit) and consumes again; returns 1, else 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_8ced0(uint32_t* p, int32_t W, int32_t H, int32_t dx,
                                                         int32_t dy, uint32_t* hit, D2cHost h) {
    int32_t x = p[3] & 0xFFFF, y = p[3] >> 16;
    p[6] = 1; p[7] = 0;
    D2cCall c = {}; c.rva = 511; c.eax = (uint32_t)(uintptr_t)p; h(&c);
    if (x < W - dx - 0x12E || x > W - dx - 0x10E) return 0;
    if (y < dy - 0x60 + H || y > dy - 0x40 + H) return 0;
    *hit = 1;
    p[6] = 1; p[7] = 0;
    D2cCall d = {}; d.rva = 511; d.eax = (uint32_t)(uintptr_t)p; h(&d);
    return 1;
}
