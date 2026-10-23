// Mapping lane: D2Client+0xC6D00 (__fastcall ecx=param_1 int*). Decompile derived; not yet vectored.
// Seam: callees via D2cHost; ordinals are passed as rva = 0x10000|ordinal (UNVERIFIED ordinal meanings).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32;
static u32 call(D2cHost h, u32 rva, u32 ecx = 0, u32 edx = 0, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0) {
    D2cCall c = {}; c.rva = rva; c.ecx = ecx; c.edx = edx; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3;
    return h(&c);
}
extern "C" {
// 1.13c D2Client+0xC6D00. p = int* (p[0]=kind, p[0xb]=handle, p[0x12]=state). tbl = base of table at RVA 0x16A608 (stride 0x80).
// Returns 1 on handled, 0 on early-out; the retail path with flag bit7 clear and bit1 clear returns an uninitialised EBX (modelled 0).
EMSCRIPTEN_KEEPALIVE u32 d2c_c6d00(D2cHost h, int32_t* p, u32 tbl) {
    const u32 O = 0x10000;
    u32 u = call(h, O | 10630);
    if (!u) return 0;
    u32 fl = call(h, O | 10397, u);
    if (fl & 0x100) {
        if (!call(h, 0xC6AA0, u)) {
            if (!(fl & 2)) return 0;
            goto L;
        }
        u32 a = call(h, O | 10179, u), b = call(h, O | 10631, u);
        u32 q = (a == 6) ? 0 : call(h, 0xA4E20, b, tbl + a * 0x80, a);
        if (!q) {
            call(h, 0xA1860, (u32)(uintptr_t)p, 0, 0, 3, 0xA783, 0xA09F0);
            p[0x12] = 0x400;
        } else {
            call(h, O | 11129);
            call(h, 0xA5FB0);
            call(h, O | 10862, (u32)(uintptr_t)p, 0);
            call(h, 0x22280, (u32)(uintptr_t)p);
            call(h, 0x88A70, 0, 0, (u32)(uintptr_t)p, q);
        }
    }
    if (!(fl & 2)) {
        if ((int8_t)fl < 0) {
            call(h, 0xC5F20, u);
            call(h, 0xC65B0, (u32)(uintptr_t)p);
            return 1;
        }
        call(h, O | 10137, u, 0x1000);
        return 0;
    }
L:
    if (p[0] != 2 && (p[0] < 4 || p[0] > 5) && p[0xb]) call(h, O | 10750, (u32)p[0xb]);
    if (p[0] != 2 && (p[0] < 4 || p[0] > 5) && p[0xb]) call(h, O | 10867, (u32)p[0xb]);
    call(h, 0xC6060);
    call(h, O | 10137, u, 0);
    return 1;
}
}
