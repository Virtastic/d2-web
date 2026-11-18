// 1.13c D2Client+0x1A060: the "unit action / skill result" packet queue. Callees go through D2cHost, in retail order.
#include "client_host.h"
#include <emscripten.h>
static const uint32_t kStk = 0xB3000000u;   // a pointer to a local out-block: its address is not part of the call
static uint32_t cl(D2cHost h, D2cCall c) { return h(&c); }

// stdcall(ctx, mode), ret 8. ctx words: +0 flags, +4 A (unit, null ok), +8 C2 (null ok; C2[0] is ebx of the 2A7C0 call in mode 1), +0xC, +0x10.
// g4c = dword D2Client+0xDBC4C, g24 = dword D2Client+0x11B824 (signed compare). A = unit words of ctx[1], inner = A[5] words.
// The 5-byte/9-byte packet bytes pushed to 143E0 live in a stack buffer and are not observable (only the call, its ebx and position are).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1a060(D2cHost h, const uint32_t* ctx, uint32_t mode, uint32_t g4c, uint32_t g24, const uint32_t* A, uint32_t* inner, const uint32_t* c2) {
    if ((int32_t)g24 > (int32_t)(g4c - 0x31u)) return;   // 1.13c D2Client+0x1A060
    uint32_t fl = ctx[0], esi = ctx[1];
    auto blk = [&](uint32_t thunk, uint32_t line, uint32_t ecx, uint32_t ebx143) {
        if (!cl(h, { 0x29F30, 0, 0, 0, 0, 0, { 0, 0, 0, 0 } })) return;
        uint32_t r1 = cl(h, { thunk, 0, 0, 0, 0, 0, { esi, 0, 0, 0 } });
        cl(h, { 0xC42A, 0, 0, 0, 0, 0, { r1, 0xB2000000u | 0xD6AD4u, line, 0 } });
        uint32_t r2 = cl(h, { thunk, 0, 0, 0, 0, 0, { esi, 0, 0, 0 } });
        cl(h, { 0xC7AE, 0, 0, 0, 0, 0, { r2, 0, 0, 0 } });
        if (cl(h, { 0x2A750, 0, ecx, esi, 0, 0, { kStk, 0, 0, 0 } })) {
            uint32_t r4 = cl(h, { 0xC250, 0, 0, 0, 0, 0, { esi, 0x148, 0, 0 } });
            cl(h, { 0xC40C, 0, 0, 0, 0, 0, { esi, 0x148, r4 + 1, 0 } });
            cl(h, { 0x143E0, 0, 0, 0, ebx143, 0, { kStk, 0, 0, 0 } });
        }
        if (esi && A && A[0] == 0 && inner) inner[0x54] = inner[0x55] = inner[0x56] = inner[0x57] = 0;
    };
    if (mode == 1) {
        if (fl & 1) {
            if (fl & 0x28) blk(0xC5BC, 0x336, 0x16, 9);   // packet type 0xA / 7 / 9 by (0x20, 8): not observable
            else cl(h, { 0x2A7C0, esi, 0, 0, c2 ? c2[0] : 6, 0, { 6, 0, 0, 0 } });
        }
        if (fl & 2) blk(0xC5CE, 0x348, 0x16, 9);   // type 0x11 / 0xE / 0x10 / 0xD by (0x20, 8)
    } else {
        if (fl & 1) {
            if (fl & 8) blk(0xC5BC, 0x32C, 0x15, 5);   // type 8
            else cl(h, { 0x2A7C0, esi, 0, 0, ctx[3], 0, { 5, 0, 0, 0 } });
        }
        if (fl & 2) blk(0xC5CE, 0x33E, 0x15, 5);   // type 0xF (8) / 0xC
    }
}
