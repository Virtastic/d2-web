// 1.13c D2Client+0xA68E0, written from the disassembly: does the screen point (x, y) hit the unit u? Stack (u, x, y, hit), ret 0x10.
// With hit == 0 it only validates u (return 1); with hit it tests the point against the unit's rectangle. gl = five pinned dwords:
// [0] D2Client+0x11C414 (panel mode 1-3), [1] +0xDBC48 (screen width), [2] +0x11B828 (an x position), [3] +0x11BBFC (playerSelf),
// [4] +0xFADB4; bm = the array behind the +0xCEE50 slot. S = the 0x28-byte stack frame (id, type, c5ec out, c65e result, scratch,
// c5ec out, rect[4]). Register callees (EDI is the J record): 0xA62A0 (A = EAX, B = stack), 0xA51F0 (EAX, EDI), 0xA5B90 and
// 0xA6070 (ESI, EDI), 0xA5440 / 0x1260 / 0x6A550 / 0x6E670 (EAX), 0xA57E0 / 0xA5720 (EAX, EBX, EDI), 0xA6730 (EAX idx, ECX unit).
// Type 5 returns 0xA57E0 raw; type 2 with a record whose +0x13A or +0x19D byte is set returns 0xA5720 raw. Retail asserts 0x741
// (Fog, then _exit) when a type-2 unit has no record; after the exit call the code falls into the type-4 test and re-dispatches on the exit stub's return, kept here.
#include "cl_env.h"
extern "C" uint32_t d2cl_a68e0(const cl::Env& e, uint32_t u, uint32_t x, uint32_t y, uint32_t hit) {
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    const uint32_t mode = e.R(e.gl), self = e.R(e.gl + 12);
    int32_t half = (int32_t)e.R(e.gl + 4); half = (half + (half < 0 ? 1 : 0)) >> 1;
    const int32_t px = (int32_t)e.R(e.gl + 8);
    if (mode == 3) return 0;
    if (mode == 1 && px >= half) return 0;
    if (mode == 2 && px < half) return 0;
    if (u && ((e.R(u + 0xc4) >> 21) & 1)) return 0;
    uint32_t ebx = self;
    uint32_t c = e.H(0xa62a0, ebx, 0, 0, 0, 0, u);
    if (!c) {
        c = e.H(0xa51f0, 0, 0, 0, 0, 0, ebx); e.Ej(0xa51f0, u);
        if (!c) {
            c = e.H(0xa5b90, 0, 0, 0, 0, 0, 0, 0, 0, 0, ebx); e.Ej(0xa5b90, u);
            if (!c) return 0;
        }
    }
    uint32_t F = 0;
    const uint32_t r = e.H(0xa6070, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); e.Ej(0xa6070, self);
    if (!u) return 0;
    const uint32_t f4 = e.R(u + 0xc4);
    if ((f4 >> 1) & 1) F = 1;
    else if (r && e.R(u) == 1 && e.H(0xa5440, 0, 0, 0, 0, 0, u) && ((f4 >> 2) & 1)) F = 1;
    if (!((e.R(u + 0xc8) >> 7) & 1) || !F) return 0;
    if (!hit) return 1;
    uint32_t S[10] = {};   // S[0] id, S[1] type (B+0x10, B+0x14), S[2] / S[5] c5ec outs, S[3] c65e, S[6..9] rect
    S[3] = e.H(0xc65e, u);
    S[0] = e.R(u + 4); S[1] = e.R(u);
    e.Xs(0x6ed);
    e.H(0xc658, u, P(&S[1]), P(&S[0]), P(&S[3]), 0xB2000000u | 0xd4144);
    const uint32_t sx = e.H(0xc1d2, u), sy = e.H(0xc1f6, u);
    const uint32_t ty = S[1];
    uint32_t edi = 0;
    const auto six = [&](uint32_t a, uint32_t b) { return e.H(0x6a550, sx, sy, a, b, 0, u); };
    const auto type4 = [&]() { return e.R(u + 0x10) == 3 && e.R(e.gl + 16) == 0; };
    if (ty <= 5) {
        if (ty == 0) { if (!six(x, y)) return 0; }
        else if (ty == 1) {
            const uint32_t row = e.H(0x1260, 0, 0, 0, 0, 0, S[0]);
            if (e.R8(e.bm) & e.R8(row + 4)) {
                const int32_t l = e.R16s(row + 0x110) + (int32_t)sx, t = e.R16s(row + 0x10e) + (int32_t)sy;
                S[6] = (uint32_t)l; S[7] = (uint32_t)t; S[8] = (uint32_t)(e.R16s(row + 0x112) + l); S[9] = (uint32_t)(e.R16s(row + 0x114) + t);
                const uint32_t hr = e.H(0xcf100, P(&S[6]), x, y);
                if (e.blk) e.blk(e.ctx, 0xcf100, P(&S[6]), 4);   // the capture records the rect after the call
                return hr ? 1 : 0;
            }
            if (!six(x, y)) return 0;
        } else if (ty == 2) {
            const uint32_t Fp = e.H(0xc226, S[0]);
            if (!six(x, y) && !(e.R8(Fp + 0x13a)) && !(e.R8(Fp + 0x19d))) return 0;
            ebx = Fp;
        } else if (ty == 4) { if (!type4()) return 0; }
        else if (ty == 5) { const uint32_t rv = e.H(0xa57e0, y, 0, 0, 0, 0, u, 0, 0, x); e.Ej(0xa57e0, sy); return rv; }
    }
    // Second switch on sel = type. The no-record assert path re-enters it at 0x6FB56B5E with EAX = the exit stub's return
    // (retail's _exit never returns; under the capture the hook does), so sel becomes that value.
    for (uint32_t sel = ty;;) {
        if (sel == 0) edi = 1;
        else if (sel == 1) { const uint32_t row = e.H(0x1260, 0, 0, 0, 0, 0, S[0]); if (row && e.R8(row + 0xec) > 1) edi = 1; }
        else if (sel == 2) {
            if (!ebx) {
                const uint32_t m = e.H(0xbf92, 0x741); e.H(0xbf80, 0xB2000000u | 0xd359f, m); const uint32_t rx = e.H(0x3664, 0xffffffffu);
                if (!type4()) return 0;
                sel = rx; continue;
            }
            const bool a13 = e.R8(ebx + 0x13a) != 0;
            const uint32_t m = e.R(u + 0x10);
            if (a13 ? (m == 2 || m == 4 || m == 5) : e.R8(ebx + 0x19d) != 0) {
                const uint32_t rv = e.H(0xa5720, y, 0, 0, 0, 0, u, 0, 0, x); e.Ej(0xa5720, ebx); return rv;
            }
            if (e.R8(ebx + 0x161) > 1) edi = 1;
        }
        break;
    }
    e.H(0xc5ec, u, P(&S[2]), P(&S[5]));
    S[6] = S[7] = S[8] = S[9] = 0;
    if (e.R(u) == 0 && e.R(u + 4) == 4 && e.R(u + 0x10) == 0x12) return 1;
    const int32_t X = (int32_t)x, Y = (int32_t)y;
    const auto inside = [&]() { return X >= (int32_t)S[6] && X <= (int32_t)S[8] && Y >= (int32_t)S[9] && Y <= (int32_t)S[7]; };
    if (!edi) return e.H(0xa6730, S[2], P(&S[6]), 0, 0, 0, 1, u) && inside() ? 1 : 0;
    for (uint32_t i = 0; i < 0x10; ++i)
        if (e.H(0xa6730, S[2], P(&S[6]), 0, 0, 0, i, u) && e.H(0x6e670, i, 0, 0, 0, 0, u) && inside()) return 1;
    return 0;
}
