// 1.13c D2Client+0x440C0 -- one-shot pass over the list headed at DAT_6FBCC3B8.
// For each node whose +0x34 bit0 is clear: 18370(node), A7D00(), kind = C1DE(node); kind 8 with DAT_6FBCC2E0 != 0 -> 53550(node); set bit0. Next node at +0x7C.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32;
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2c_440c0(D2cHost h, u32 g3b8, u32 g2e0) {
    D2cCall c = {}; c.rva = 0xCCC4; c.s[0] = g3b8; u32 n = h(&c);
    for (; n; n = *(u32*)(uintptr_t)(n + 0x7c)) {
        u32* fl = (u32*)(uintptr_t)(n + 0x34);
        if (*fl & 1) continue;
        D2cCall a = {}; a.rva = 0x18370; a.s[0] = n; h(&a);
        D2cCall b = {}; b.rva = 0xA7D00; h(&b);
        D2cCall k = {}; k.rva = 0xC1DE; k.s[0] = n;
        if (h(&k) == 8 && g2e0) { D2cCall d = {}; d.rva = 0x53550; d.s[0] = n; h(&d); }
        *fl |= 1;
    }
}
}
