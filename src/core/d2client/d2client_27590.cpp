// 1.13c D2Client+0x27590 life / mana orb hover numbers (no arguments, void). Globals (values in G): mouse x 0x11B828, y 0x11B824,
// viewed unit 0x11BBFC, life / mana pinned flags 0x11C4B0 / 0x11C4B4, screen width 0xDBC48, height 0xDBC4C. Four zeroed 100-char
// Unicode buffers (D2Lang ??_FUnicode via 0xCEB00): A (life text), B (mana text), C, D (sprintf scratch). Life: flag set or x in
// [0x1E, 0x6E] and y in [h-0x4B, h-0xF]: cur = 0x26B20(EAX 6) >> 8, max = 0x0C196(unit) >> 8 (D2Common), a type-0 unit whose class
// (+0x10) is not 0x11 with cur in [0, 1] shows cur 1; Unicode::sprintf (0xCEAB4, cdecl)(100, C, 0xD1EC(ECX 0x1045), cur, max),
// Unicode::strcat (0xCEABC; ECX A, EDX C), w = 0xD35A(ECX A), 0xD37E(ECX A, EDX 0x41 - w/2; h-0x5F, 0, 0). Mana: flag set or x in
// [W-0x6F, W-0x1F], same y band: 0x26B20(EAX 8), 0x0CF4C(unit), string 0x1046, D into B, x = W - w/2 - 0x50.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

struct D2c27590G { int32_t x, y; uint32_t unit; uint32_t flagL, flagM; int32_t w, h; };
static uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_27590(const D2cSvc& v, const D2c27590G* g) {
    char16_t A[100] = {}, B[100] = {}, C[100] = {}, D[100] = {};
    const uint32_t unit = g->unit;
    const int32_t x = g->x, y = g->y;
    const bool yin = y >= g->h - 0x4b && y <= g->h - 0xf;
    if (g->flagL || (x >= 0x1e && x <= 0x6e && yin)) {
        int32_t cur = (int32_t)d2c_call(v, 0x26b20, 6, 0, 0, 0, 0) >> 8;
        const int32_t mx = (int32_t)d2c_call(v, 0xc196, 0, 0, 0, 0, 0, unit) >> 8;
        if (g->unit) {
            uint32_t t, c; memcpy(&t, (const void*)(uintptr_t)g->unit, 4); memcpy(&c, (const void*)(uintptr_t)(g->unit + 0x10), 4);
            if (t == 0 && c != 0x11 && cur >= 0 && cur <= 1) cur = 1;
        }
        const uint32_t fmt = d2c_call(v, 0xd1ec, 0, 0x1045, 0, 0, 0);
        d2c_call(v, 0xceab4, 0, 0, 0, 0, 0, 100, P(C), fmt, (uint32_t)cur, (uint32_t)mx);
        d2c_call(v, 0xceabc, 0, P(A), P(C), 0, 0);
        const int32_t w = (int32_t)d2c_call(v, 0xd35a, 0, P(A), 0, 0, 0);
        d2c_call(v, 0xd37e, 0, P(A), (uint32_t)(0x41 - w / 2), 0, 0, (uint32_t)(g->h - 0x5f), 0, 0);
    }
    if (g->flagM || (x >= g->w - 0x6f && x <= g->w - 0x1f && yin)) {
        const int32_t cur = (int32_t)d2c_call(v, 0x26b20, 8, 0, 0, 0, 0) >> 8;
        const int32_t mx = (int32_t)d2c_call(v, 0xcf4c, 0, 0, 0, 0, 0, unit) >> 8;
        const uint32_t fmt = d2c_call(v, 0xd1ec, 0, 0x1046, 0, 0, 0);
        d2c_call(v, 0xceab4, 0, 0, 0, 0, 0, 100, P(D), fmt, (uint32_t)cur, (uint32_t)mx);
        d2c_call(v, 0xceabc, 0, P(B), P(D), 0, 0);
        const int32_t w = (int32_t)d2c_call(v, 0xd35a, 0, P(B), 0, 0, 0);
        d2c_call(v, 0xd37e, 0, P(B), (uint32_t)(g->w - w / 2 - 0x50), 0, 0, (uint32_t)(g->h - 0x5f), 0, 0);
    }
}
