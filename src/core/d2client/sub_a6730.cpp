// src/core/d2client/sub_a6730.cpp -- D2Client.dll 1.13c, base 0x6FAB0000. Env-seam: callees via D2cHost by rva.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"

static uint32_t hc(D2cHost h, uint32_t rva, uint32_t eax = 0, uint32_t ecx = 0, uint32_t esi = 0,
                   uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = eax; c.ecx = ecx; c.esi = esi; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2;
    return h(&c);
}

// 1.13c D2Client+0xA6730 (417 B, thiscall ECX=this, EAX=idx, stack frame, out rect[4]; ret 8; caller 0xA68E0):
// cel bounding rect of a unit/object. Local 0x48-B struct L: L[0]=frame, byte L+5=idx, L+0x30=count (set by
// D2CMP#10005), L+0x3c=cel ptr (same), L+0x40=D2Common#10663(this)&0xff, L+0x34=0.
// Returns 0 if 6DA40(idx,this)==0 / 6E2C0 fails / 10005 fails / no cel. frame>count: assert 0x5df then _exit.
// out[0]=10002(cel)+10651(this)-0xC; out[2]=out[0]+10060(cel)+0x18; out[1]=10069(cel)+11142(this)+0xC;
// out[3]=out[1]-10000(cel)-0x18. Width<0x20 widens each side by (0x20-w)>>1; same for height. Then
// x += C7F60(this); y += C7F40(this)+C7F20(this).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a6730(uint32_t self, uint32_t idx, uint32_t frame,
                                                         int32_t* out, D2cHost h) {
    if (!hc(h, 0x6DA40, idx, 0, self)) return 0;
    uint32_t L[18] = {};
    uint8_t* Lb = (uint8_t*)L;
    L[0x40 / 4] = hc(h, 0xC610, 0, 0, self) & 0xff;
    L[0] = frame;
    L[0x34 / 4] = 0;
    Lb[5] = (uint8_t)idx;
    uint32_t pout = (uint32_t)(uintptr_t)&out;
    if (!hc(h, 0x6E2C0, 0xFFFFFFFFu, 0, self, self, (uint32_t)(uintptr_t)L, pout)) return 0;
    if (!hc(h, 0xC0D0, 0, 0, 0, (uint32_t)(uintptr_t)L, 0, 1)) return 0;
    uint32_t cel = L[0x3c / 4];
    if (!cel) return 0;
    if (frame > L[0x30 / 4]) {
        uint32_t m = hc(h, 0xBF92, 0, 0, 0, 0x5df);
        hc(h, 0xBF80, 0, 0, 0, 0x6fb8359f, m);
        hc(h, 0x3664, 0, 0, 0, 0xFFFFFFFFu);
        return 0;  // _exit: not reached in retail
    }
    int32_t a = (int32_t)hc(h, 0xC0CA, 0, 0, 0, cel);
    out[0] = a + (int32_t)hc(h, 0xC1D2, 0, 0, 0, self) - 0xC;
    out[2] = (int32_t)hc(h, 0xC0D6, 0, 0, 0, cel) + out[0] + 0x18;
    int32_t b = (int32_t)hc(h, 0xC0DC, 0, 0, 0, cel);
    int32_t y1 = b + (int32_t)hc(h, 0xC1F6, 0, 0, 0, self) + 0xC;
    out[1] = y1;
    out[3] = y1 - (int32_t)hc(h, 0xC0E2, 0, 0, 0, cel) - 0x18;
    int32_t w = out[2] - out[0];
    if (w < 0x20) { int32_t d = (0x20 - w) >> 1; out[2] += d; out[0] -= d; }
    int32_t hh = y1 - out[3];
    if (hh < 0x20) { int32_t d = (0x20 - hh) >> 1; out[1] = y1 + d; out[3] -= d; }
    int32_t dx = (int32_t)hc(h, 0xC7F60, self), dy = (int32_t)hc(h, 0xC7F40, self), dz = (int32_t)hc(h, 0xC7F20, self);
    out[0] += dx; out[2] += dx;
    out[3] += dy + dz; out[1] += dy + dz;
    return 1;
}
