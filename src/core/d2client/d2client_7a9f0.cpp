// src/core/d2client/d2client_7a9f0.cpp -- D2Client+0x7A9F0. EAX = ctx (register arg), one stack arg
// (skip-units flag, stored as frame[6]). Every callee is a host function in D2Client7a9f0Env; the globals
// 0x6FBC3B18..0x6FBC3B30 are the seven words at `gl`. Recovered from the disassembly: the Ordinal_11152 frame is
// seven words on the caller's stack [ctx, left, top, right, bottom, player room, skip flag].
#include <cstdint>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static inline void wr32(uint32_t a, uint32_t v) { *(uint32_t*)(uintptr_t)a = v; }

struct D2Client7a9f0Env {
    void (*sub_7a0d0)(uint32_t ctx);                                  // D2Client+0x7A0D0 (ESI = ctx)
    int32_t (*ord10013)(void);                                        // D2Gfx 10013 via thunk 0xD07E
    void (*ord10512)(int32_t x, int32_t y, int32_t* ox, int32_t* oy); // D2Common 10512 via 0xCB2C
    uint32_t (*room1)(uint32_t unit);                                 // 0xC1F0, one stack word
    uint32_t (*room3)(uint32_t unit, uint32_t x, uint32_t y);         // 0xC1F0, three stack words
    uint32_t (*ord10867)(uint32_t path);                              // 0xC3BE
    uint32_t (*ord10750)(uint32_t path);                              // 0xC2EC
    uint32_t (*ord10916)(uint32_t room);                              // 0xCB14
    void (*ord11152)(uint32_t ctx, uint32_t cb_rva, uint32_t* frame); // 0xCB1A (cb = D2Client+0x7A5D0)
    void (*fatal)(uint32_t line);                                     // Fog 10265 + 10024 + _exit(-1): the no-table and no-unit exits
};

// Types 2, 4 and 5 read the path word directly; others call the ordinal on the path (a null path gives 0).
static uint32_t pos_of(uint32_t u, uint32_t off, uint32_t (*ord)(uint32_t)) {
    uint32_t s = rd32(u + 0x2c);
    int32_t t = (int32_t)rd32(u);
    if (t == 2 || (t > 3 && t < 6)) return rd32(s + off);
    return s ? ord(s) : 0;
}

// 1.13c D2Client+0x7A9F0 (637 B)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7a9f0(uint32_t ctx, uint32_t skip, uint32_t* gl /*3b18 3b1c 3b20 3b24 3b28 3b2c 3b30*/, uint32_t unit,
                     const D2Client7a9f0Env* env) {
    env->sub_7a0d0(ctx);
    int32_t big = env->ord10013();
    int32_t x = (int32_t)rd32(ctx + 0x24), y = (int32_t)rd32(ctx + 0x28);
    int32_t w = (int32_t)(rd32(ctx + 0xc) - rd32(ctx + 4)), h = (int32_t)(rd32(ctx + 0x10) - rd32(ctx + 8));
    int32_t l = x, t = y;
    if (big) { w += 0x30; l -= 0x30; t -= 0x30; }
    uint32_t frame[7] = { ctx, (uint32_t)l, (uint32_t)t, (uint32_t)(w + x), (uint32_t)(h + y), 0, skip };
    gl[3] = frame[1]; gl[4] = frame[2]; gl[5] = frame[3]; gl[6] = frame[4];
    wr32(ctx + 0xea9c, 0);
    wr32(ctx + 0x38, 0);
    uint32_t tab = rd32(ctx + 0xeaa8);
    if (!tab) { env->fatal(0x33c); return; }
    for (uint32_t n = rd32(ctx + 0xeaac) * 9 & 0x3fffffff, p = tab; n; n--, p += 4) wr32(p, 0);
    int32_t ox, oy;
    env->ord10512(x, y, &ox, &oy);
    oy -= (int32_t)rd32(ctx + 0xeab4);
    ox -= 3;
    wr32(ctx + 0xeaa4, (uint32_t)oy);
    wr32(ctx + 0xeaa0, (uint32_t)ox);
    uint32_t e = tab, dim = rd32(ctx + 0xeab0);
    for (uint32_t r = 0; r < dim; r++)
        for (uint32_t c = 0; c < dim; c++, e += 0x24) {
            wr32(e + 8, (uint32_t)ox + c);
            wr32(e + 0xc, (uint32_t)oy + r);
        }
    if (!unit) { env->fatal(0x351); return; }
    frame[5] = env->room1(unit);
    gl[0] = pos_of(unit, 0xc, env->ord10867) / 5;
    gl[2] = pos_of(unit, 0x10, env->ord10750) / 5;
    uint32_t py = pos_of(unit, 0x10, env->ord10750);
    uint32_t px = pos_of(unit, 0xc, env->ord10867);
    gl[1] = env->ord10916(env->room3(unit, px, py));
    env->ord11152(ctx, 0x7A5D0, frame);
}
