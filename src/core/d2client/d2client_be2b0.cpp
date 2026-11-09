// 1.13c D2Client+0xBE2B0 postMouseMoveMessage: SendMessageA(D2gfx#10048(), WM_MOUSEMOVE, 0, MAKELONG(y16, x16)) from the words at +0x11B824 (x) and +0x11B828 (y).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_be2b0(D2cHost h, uint32_t cursX, uint32_t cursY) {
    D2cCall c = {}; c.rva = 0xD0B4; uint32_t hwnd = h(&c);              // D2gfx #10048 (window handle)
    c = {}; c.rva = 0xCF118;                                             // USER32 SendMessageA (IAT slot)
    c.s[0] = hwnd; c.s[1] = 0x200; c.s[2] = 0; c.s[3] = (cursX & 0xffff) << 16 | (cursY & 0xffff);
    h(&c);
}
