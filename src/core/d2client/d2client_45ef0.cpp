// D2Client 0x45EF0: the buy/sell panel reset (caller 0x46A20).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
static void wr45ef0(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }
// 1.13c D2Client+0x45EF0 ; g = 0x119660..0x1197C7 (0x160-byte block, then the cels 0x1197C0/0x1197C4; 0x1197B4/0x1197B8 inside it),
// h = 0x1222F8..0x12230C. Every store is a dword except 0x1222F8 (a byte). The GetTickCount result is discarded (not modelled).
// Host: 0xCE02 (#10939, 1 stack word), 0x3146 (sprintf, cdecl: buf, format, "DATA\GLOBAL" as module RVAs), 0x2B420 (cel load, EAX = path, 1 word).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_45ef0(uint8_t* g, uint8_t* h, D2cHost host) {
    wr45ef0(g, 0x158, 0); wr45ef0(g, 0x154, 0);
    std::memset(g, 0, 0x160);
    D2cCall c = {}; c.rva = 0xCE02; wr45ef0(g, 0xDB, host(&c));
    c = {}; c.rva = 0xCE02; wr45ef0(g, 0xDF, host(&c));
    wr45ef0(g, 3, 0);
    static const uint32_t fmt[2] = { 0xD6078, 0xD6064 };   // "%s\ui\panel\buysell", "%s\ui\menu\goldbtn"
    uint32_t cel[2];
    for (int i = 0; i < 2; ++i) {
        char buf[0x104];
        c = {}; c.rva = 0x3146; c.s[0] = (uint32_t)(uintptr_t)buf; c.s[1] = fmt[i]; c.s[2] = 0xD357C; host(&c);
        c = {}; c.rva = 0x2B420; c.eax = (uint32_t)(uintptr_t)buf; cel[i] = host(&c);
        if (i == 0) wr45ef0(g, 0x160, cel[0]);
    }
    wr45ef0(h, 0xD, 0); h[0] = 0; wr45ef0(h, 0x11, 0); wr45ef0(h, 9, 0); wr45ef0(g, 0x164, cel[1]);
    return cel[1];   // EAX on return still holds the second cel
}
