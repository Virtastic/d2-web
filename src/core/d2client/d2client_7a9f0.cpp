// src/core/d2client/d2client_7a9f0.cpp -- D2Client+0x7A9F0 (row s3562). EAX = ctx (register arg), one stack arg
// (param_1, stored in a local and never read). Globals are pointers, ordinals/unrecovered callees are host
// callbacks. GUESSED: ordinal args hidden in registers are passed as the decompile shows them.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static inline void wr32(uint32_t a, uint32_t v) { *(uint32_t*)(uintptr_t)a = v; }

// unit-type test shared by the three position reads: types 2,4,5 read the +0xb slot directly.
static uint32_t pos_of(uint32_t u, uint32_t off, uint32_t (*ord)(uint32_t)) {
    uint32_t s = rd32(u + 0x2c);
    int32_t t = (int32_t)rd32(u);
    if (t == 2 || (t > 3 && t < 6)) return rd32(s + off);
    return s ? ord(s) : 0;
}

// 1.13c D2Client+0x7A9F0 (637 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7a9f0(uint32_t ctx, uint32_t* rect_3b24 /*4 words*/, uint32_t* g_3b18, uint32_t* g_3b1c,
                     uint32_t* g_3b20, uint32_t unit_bbfc,
                     int32_t (*sub_7a0d0)(uint32_t), int32_t (*ord10013)(void),
                     void (*ord10512)(uint32_t, uint32_t, int32_t*), uint32_t (*ord10331)(void),
                     uint32_t (*ord10867)(uint32_t), uint32_t (*ord10750)(uint32_t),
                     uint32_t (*ord10331_3)(uint32_t, uint32_t, uint32_t), uint32_t (*ord10916)(uint32_t),
                     void (*ord11152)(uint32_t, void*, int32_t*), void* sub_7a5d0,
                     uint32_t (*ord10265)(void), void (*ord10024)(const void*, uint32_t),
                     const void* str_359f, void (*exit_)(int)) {
    sub_7a0d0(ctx);
    int32_t big = ord10013();
    int32_t x = (int32_t)rd32(ctx + 0x24), y = (int32_t)rd32(ctx + 0x28);
    int32_t w = (int32_t)(rd32(ctx + 0xc) - rd32(ctx + 4)), h = (int32_t)(rd32(ctx + 0x10) - rd32(ctx + 8));
    int32_t l = x, t = y;
    if (big) { w += 0x30; l -= 0x30; t -= 0x30; }
    rect_3b24[0] = l; rect_3b24[1] = t; rect_3b24[2] = w + x; rect_3b24[3] = h + y;
    wr32(ctx + 0xea9c, 0);
    wr32(ctx + 0x38, 0);
    uint32_t tab = rd32(ctx + 0xeaa8);
    if (tab) {
        for (uint32_t n = rd32(ctx + 0xeaac) * 9 & 0x3fffffff, p = tab; n; n--, p += 4) wr32(p, 0);
        int32_t o[2];
        ord10512(rd32(ctx + 0x24), rd32(ctx + 0x28), o);
        o[1] -= (int32_t)rd32(ctx + 0xeab4);
        o[0] -= 3;
        wr32(ctx + 0xeaa4, o[1]);
        wr32(ctx + 0xeaa0, o[0]);
        uint32_t e = tab, dim = rd32(ctx + 0xeab0);
        for (uint32_t r = 0; dim && r < dim; r++)
            for (uint32_t c = 0; c < dim; c++, e += 0x24) {
                wr32(e + 8, (uint32_t)o[0] + c);
                wr32(e + 0xc, (uint32_t)o[1] + r);
            }
        if (unit_bbfc) {
            ord10331();
            uint32_t u = rd32(unit_bbfc + 0x2c);
            (void)u;
            uint32_t a = pos_of(unit_bbfc, 0xc, ord10867);
            *g_3b18 = a / 5;
            uint32_t b = pos_of(unit_bbfc, 0x10, ord10750);
            *g_3b20 = b / 5;
            *g_3b1c = ord10916(ord10331_3(unit_bbfc, pos_of(unit_bbfc, 0xc, ord10867), pos_of(unit_bbfc, 0x10, ord10750)));
            // stack0xffffffd0 is the frame base the callee walks; hand it the local out-pair's frame.
            ord11152(ctx, sub_7a5d0, o);
            return;
        }
    }
    ord10024(str_359f, ord10265());
    exit_(-1);
}
