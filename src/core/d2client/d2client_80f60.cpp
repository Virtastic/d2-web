// 1.13c D2Client+0x80F60: the weather particle step. stdcall(1): the stack word is handed on to 0x80700.
// Floats are x87: the products are exact in extended precision, so they are formed in long double and truncated (FUN_6fb7acc4).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "d2client_80f60.h"
namespace {
template <class T> T rd(const uint8_t* p, uint32_t o) { T v; memcpy(&v, p + o, sizeof v); return v; }
template <class T> void wr(uint8_t* p, uint32_t o, T v) { memcpy(p + o, &v, sizeof v); }
int32_t ftol(long double v) { return (int32_t)v; }
}
// 1.13c D2Client+0x80F60
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_80f60(D2c80f60G* g, const D2c80f60Env* e, uint32_t param) {
    float k;
    if (g->rainMode) {
        float c = e->cosF((int32_t)g->angle); if (c < 0) c = -c;
        k = c < 0.25f ? 0.25f : c;
    } else k = (float)((long double)g->f38f4 * 0.15f + 0.85f);
    uint8_t* p = rd<uint8_t*>(g->ctx, 0x11c);
    for (int32_t i = 0; i <= rd<int32_t>(g->ctx, 0x110); i++, p += 0x28) {
        if (rd<int16_t>(p, 0) == 0) continue;
        const int32_t w1 = ftol((long double)rd<int32_t>(p, 0x14) * k);
        const float f1 = (float)w1;
        int32_t edi = ftol((long double)e->cosF((int32_t)g->angle) * f1);
        const int32_t s = ftol((long double)e->sinF((int32_t)g->angle) * f1);
        const int32_t y = rd<int32_t>(p, 8) + (s - g->tickDY);
        wr<int32_t>(p, 8, y);
        if (y > rd<int32_t>(p, 0xc)) wr<int32_t>(p, 0x1c, 1);
        if (rd<int32_t>(p, 0x1c) != 0) {
            if (rd<int32_t>(p, 0x20) == 0) {
                e->ord10065((uint32_t)(uintptr_t)g->ctx, (uint32_t)i << 16);
                if (rd<int32_t>(g->ctx, 0x114) < g->maxLive) e->fn80700(param);
            } else {
                edi = 0; wr<int32_t>(p, 0x14, 0);
                wr<int32_t>(p, 0x20, rd<int32_t>(p, 0x20) - 1);
            }
        }
        if (rd<int16_t>(p, 0) != 0) {
            if (g->rainMode && rd<int32_t>(p, 0x1c) == 0) {
                const uint32_t t = (((uint32_t)g->frame << 9) / 25 + (uint32_t)rd<int32_t>(p, 0x18)) & 0x1ff;
                edi += ftol(2.0L * e->sinF((int32_t)t));
            }
            int32_t x = rd<int32_t>(p, 4) - g->tickDX + edi;
            if (x >= g->cellW) x -= g->cellW;
            if (x < 0) x += g->cellW;
            wr<int32_t>(p, 4, x);
        }
    }
    g->frame++;
}
