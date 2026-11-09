// 1.13c D2Client+0xC07C0 tooltipLatch (EAX = id, one stack flag, ret 4): unless the dword at +0x119721 is set, 0xC0590 supplies a colour; flag set: 0xA5D90(EAX = id) text is copied to the message buffer
// at +0x11C980 and the anchor words +0x11CA20 = screen +0xDBC54, +0x11CA24 = +0xDBC58 - 0x48, +0x11CA28 = colour are latched (empty buffer when no text); flag clear: D2Win #10070 draws the text
// at (+0xDBC54, +0xDBC58 - 0x48) in that colour. Unicode::strcpy is a pure D2Lang import (native). msg = the 0xAC bytes at +0x11C980.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_c07c0(D2cHost h, uint32_t eax, uint32_t flag, uint32_t g9721, int32_t scr54, int32_t scr58, uint16_t* msg) {
    if (g9721) return;
    D2cCall c = {}; c.rva = 0xC0590; uint32_t col = h(&c);
    int32_t y = scr58 - 0x48;
    if (flag) {
        c = {}; c.rva = 0xA5D90; c.eax = eax; const uint16_t* t = (const uint16_t*)(uintptr_t)h(&c);
        msg[0] = 0;
        if (!t) return;
        for (uint32_t i = 0; ; i++) { msg[i] = t[i]; if (!t[i]) break; }
        uint32_t* a = (uint32_t*)((uint8_t*)msg + 0xa0);
        a[1] = (uint32_t)y; a[2] = col; a[0] = (uint32_t)scr54;
        return;
    }
    c = {}; c.rva = 0xA5D90; c.eax = eax; uint32_t t = h(&c);
    c = {}; c.rva = 0xD360; c.ecx = t; c.edx = (uint32_t)scr54; c.s[0] = (uint32_t)y; c.s[1] = 0; c.s[2] = 1; c.s[3] = col; h(&c);
}
