// 1.13c D2Client+0xC7290 -- mapping lane, decompile derived; not yet vectored.
// Seam: imported calls go through D2cHost with call.rva = D2Common/D2Gfx ordinal (ordinal-to-DLL unverified), in retail order.
// `self` is the implicit EAX arg: self[0]=kind, self[0xb]=unit ptr. D2cHost for FUN_6fb57e20/6fb76060 uses their RVAs.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32;
static u32 ord(D2cHost h, u32 o, u32 a = 0) { D2cCall c = {}; c.rva = o; c.eax = a; return h(&c); }
extern "C" {
// 1.13c D2Client+0xC7290
EMSCRIPTEN_KEEPALIVE void d2c_c7290(D2cHost h, u32* self, u32* unit /*= self[0xb] deref'd as u32[]*/) {
    u32 kind = self[0];
    // retail: cmp (-(kind!=1) & 0xffffff80) + 0x100 -> 0x80 if kind!=1 else 0x100
    u32 want = (kind != 1) ? 0x80u : 0x100u;
    if (ord(h, 10259, self[0xb]) == want) return;
    u32 r = ord(h, 10511), x, y;
    bool direct = (kind == 2) || (kind > 3 && kind < 6);
    if (r == 0) x = direct ? unit[0xc / 4] : (self[0xb] == 0 ? 0 : ord(h, 10867, self[0xb]));
    else x = ord(h, 10980, r);
    if (r == 0) y = direct ? unit[0x10 / 4] : (self[0xb] == 0 ? 0 : ord(h, 10750, self[0xb]));
    else y = ord(h, 10649, r);
    ord(h, 10123);
    u32 t = ord(h, 10331);
    ord(h, 11073, t);
    D2cCall c = {}; c.rva = 0x57E20; c.eax = x; c.edx = y; c.s[0] = 0; h(&c);  // FUN_6fb57e20(x,y,0): reg convention unverified
    c = {}; c.rva = 0x76060; h(&c);
}
}
