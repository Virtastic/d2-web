// 1.13c D2Client+0xA6730, written from the disassembly: the screen rectangle of a unit's cel. EAX = idx, ECX = this (unit),
// stack (frame, out int[4]), ret 8. 0x48-byte local L: L[0] = frame, byte L+5 = idx, L+0x30 = frame count and L+0x3c = cel (both
// filled by the cel-table call 0xC0D0), L+0x34 = 0, L+0x40 = (0xC610 this) & 0xff. 0x6E2C0 gets (this, &L, &out-argument-slot)
// with EAX = -1; its block L is the capture's hookblk. Calls 0xBF92 / 0xBF80 are cdecl, the rest stdcall or EAX-register thunks.
#include "cl_env.h"
extern "C" uint32_t d2cl_a6730(const cl::Env& e, uint32_t idx, uint32_t self, uint32_t frame, uint32_t out) {
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    if (!e.H(0x6da40, 0, 0, 0, 0, 0, idx, 0, 0, 0, self)) return 0;
    uint32_t L[18] = {};
    L[0x40 / 4] = e.H(0xc610, self) & 0xff;
    L[0] = frame;
    L[0x34 / 4] = 0;
    reinterpret_cast<uint8_t*>(L)[5] = (uint8_t)idx;
    uint32_t slot = out;
    const uint32_t got = e.H(0x6e2c0, self, P(L), P(&slot), 0, 0, 0xFFFFFFFFu);
    if (e.blk) e.blk(e.ctx, 0x6e2c0, P(L), 18);   // the capture records the block after every call, a zero result included
    if (!got) return 0;
    if (!e.H(0xc0d0, P(L), 0, 1)) return 0;
    const uint32_t cel = L[0x3c / 4];
    if (!cel) return 0;
    if (frame > L[0x30 / 4]) {
        const uint32_t m = e.H(0xbf92, 0x5df);
        e.H(0xbf80, 0xB2000000u | 0xd359f, m);   // the source-file string, recorded module-relative
        e.H(0x3664, 0xffffffffu);
        return 0;
    }
    const int32_t a = (int32_t)e.H(0xc0ca, cel);
    const int32_t b = (int32_t)e.H(0xc1d2, self);
    const int32_t x0 = a + b - 0xc;
    e.W(out, (uint32_t)x0);
    int32_t x1 = (int32_t)e.H(0xc0d6, cel) + x0 + 0x18;
    e.W(out + 8, (uint32_t)x1);
    const int32_t c = (int32_t)e.H(0xc0dc, cel);
    const int32_t d = (int32_t)e.H(0xc1f6, self);
    int32_t y1 = c + d + 0xc;
    e.W(out + 4, (uint32_t)y1);
    int32_t y0 = y1 - (int32_t)e.H(0xc0e2, cel) - 0x18;
    e.W(out + 12, (uint32_t)y0);
    int32_t w = x1 - x0;
    int32_t xl = x0;
    if (w < 0x20) {
        const int32_t k = (0x20 - w) >> 1;
        x1 += k; xl = x0 - k;
        e.W(out + 8, (uint32_t)x1);
        e.W(out, (uint32_t)xl);
    }
    const int32_t h = y1 - y0;
    if (h < 0x20) {
        const int32_t k = (0x20 - h) >> 1;
        y1 += k; y0 -= k;
        e.W(out + 4, (uint32_t)y1);
        e.W(out + 12, (uint32_t)y0);
    }
    const int32_t dx = (int32_t)e.H(0xc7f60, 0, 0, 0, 0, 0, self);
    const int32_t dy = (int32_t)e.H(0xc7f40, 0, 0, 0, 0, 0, self);
    const int32_t dz = (int32_t)e.H(0xc7f20, 0, 0, 0, 0, 0, self);
    e.W(out, (uint32_t)(xl + dx));
    e.W(out + 8, (uint32_t)(x1 + dx));
    e.W(out + 4, (uint32_t)(y1 + dy + dz));
    e.W(out + 12, (uint32_t)(y0 + dy + dz));
    return 1;
}
