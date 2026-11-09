// 1.13c D2Client+0xC0590 pendingEventScale (EDI = unit): scale 0..0x80 of the unit's pending-event progress, 0 for a non-player, a flagged or a mode 0/0xC unit.
// Unit +0x0 type, +0xC id, +0x10 mode, +0xC4 flags. Bit 2 of the flags: 0xA62A0(EAX = unit, stack = the local player at +0x11BBFC); a nonzero result sets the gate EDX = 1
// (21580 leaves EDX alone). With gate 0 the +0x21580 list walk (head +0x11C4D4; node +8 id, +0x1C percent, +0x30 next) gives a percent; percent > 0 scales (percent<<7)/100.
// Otherwise D2Common #10913(unit) is the divisor and #10973(unit, 6, 0)<<7 over it the scale (divisor 0 keeps 0x80). Under 3 gives 2; the result caps at 0x80.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2c_c0590(D2cHost h, const uint8_t* u, uint32_t player, const uint8_t* list) {
    if (!u) return 0;
    uint32_t type = *(const uint32_t*)u, flags = *(const uint32_t*)(u + 0xc4), mode = *(const uint32_t*)(u + 0x10);
    if (type != 1 || (flags & 0x10000) || mode == 0 || mode == 0xc) return 0;
    uint32_t ua = (uint32_t)(uintptr_t)u;
    int32_t s = 0x80; uint32_t gate = 0;
    if ((flags >> 2) & 1) { D2cCall c = {}; c.rva = 0xA62A0; c.eax = ua; c.s[0] = player; gate = h(&c) ? 1 : 0; }
    int32_t pct = 0;
    for (const uint8_t* n = list; n; n = (const uint8_t*)(uintptr_t)*(const uint32_t*)(n + 0x30))
        if (*(const uint32_t*)(n + 8) == *(const uint32_t*)(u + 0xc)) { pct = (int32_t)*(const uint32_t*)(n + 0x1c); break; }
    if (!gate && pct > 0) s = (int32_t)((int64_t)(int32_t)((uint32_t)pct << 7) / 100);
    else {
        D2cCall c = {}; c.rva = 0xC196; c.s[0] = ua; int32_t d = (int32_t)h(&c);
        if (d) {
            c = {}; c.rva = 0xC1C6; c.s[0] = ua; c.s[1] = 6; c.s[2] = 0; int32_t v = (int32_t)h(&c);
            s = (int32_t)((int64_t)(int32_t)((uint32_t)v << 7) / d);
        } else return 0x80;
    }
    if (s <= 2) return 2;
    return s < 0x80 ? s : 0x80;
}
