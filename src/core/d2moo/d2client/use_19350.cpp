// 1.13c D2Client+0x19350: the "use skill" gate/prepare step called first by 0x1B7F0.
// ESI = ctx (words: +0 flags, +4 A unit, +8 B, +0xC/+0x10 out, +0x14/+0x18 c5/c6, +0x1C c7). Callees go through D2cHost, in retail order.
// gm: words 0 and 4 are the first bytes of the 0xCEE50 mask table at +0 and +0x10; word 9 = the mode global 0x11B864.
#include "client_host.h"
#include <emscripten.h>
static const uint32_t kStk = 0xB3000000u;
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_19350(D2cHost h, uint32_t* ctx, uint32_t ctxTok, const uint32_t* A, const uint32_t* inner, const uint32_t* gm) {
    const uint32_t flags = ctx[0], a = ctx[1], c7 = ctx[7];
    auto go = [&](uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3) {
        D2cCall k = { rva, eax, ecx, edx, ebx, 0, { s0, s1, s2, s3 } }; return h(&k); };
    uint32_t kind = go(0xD000, 0, 0, 0, 0, a, c7, 0, 0);
    if (c7 && (flags & 2) && kind != 1) {
        D2cCall q = { 0xC640, 0, c7, 0, 0, 0, { 0, 0, 0, 0 } };
        if (!h(&q)) return 0;
        uint32_t p5 = q.out[0] & 0xFF, p4 = (q.out[0] >> 8) & 0xFF;   // p[5], p[4] of the record
        uint32_t u = go(0xC1F0, 0, 0, 0, 0, a, 0, 0, 0);
        if (!(gm[0] & p5) && go(0xC208, 0, 0, 0, 0, u, 0, 0, 0)) { go(0x23200, 0, 0, 0, 0, a, 0x18, 0, 0); return 0; }
        if (gm[4] & p4) return 0;
    }
    ctx[5] = (flags & 0x40) ? 3 : 1; ctx[6] = (flags & 0x40) ? 4 : 2;
    if (A && A[0] == 0 && inner && inner[0x55]) {
        bool call16a0 = true;
        if (inner[0x55] == 0x13) {
            uint32_t s = inner[0x56], v = inner[0x57];
            call16a0 = go(0xA4E20, v & 0x7F, v, 0xB2000000u | (0x10A608u + s * 0x200u), 0, s, 0, 0, 0) != 0;
        }
        if (call16a0) go(0x16A0, a, 0, 0, 0, 0, 0, 0, 0);
    }
    if (!c7) return 0;
    ctx[2] = go(0x51AE0, ctx[2], 0, 0, 0, c7, ctxTok + 0xC, ctxTok + 0x10, flags);   // 5th stack word m = flags>>5&1, or 1 when flags&2 and B==0: not logged by the capture
    if (gm[9] == 6) go(0xBEB90, 0, 0, 0, 0, 0, 0, 0, 0);
    else if (gm[9] == 8) go(0x143E0, 0, 0, 0, 5, kStk, 0, 0, 0);
    return 1;
}
