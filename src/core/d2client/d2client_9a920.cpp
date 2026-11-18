// 1.13c D2Client+0x9A920: left-button-down on the HUD bar (stack arg = the input message; the cursor comes from the globals
// D2Client+0x11B828 (x) / +0x11B824 (y); ret 4). Env-seam style as d2client_98dc0.cpp:
// `h` calls a retail callee at its RVA with the registers (eax ecx edx ebx esi edi) and stack words its call site loads;
// `rd`/`wr` reach memory by virtual address (module globals at D2Client base 0x6FAB0000).
#include <cstdint>
#include <cstring>
struct D2c9a920Env {
    void* c;
    uint32_t (*rd)(void* c, uint32_t va);
    void (*wr)(void* c, uint32_t va, uint32_t v);
    uint32_t (*h)(void* c, uint32_t rva, uint32_t a, uint32_t cx, uint32_t d, uint32_t b, uint32_t s, uint32_t di,
                  uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6);
};
// 1.13c D2Client+0x9A920
extern "C" void d2c_9a920(const D2c9a920Env& e, uint8_t* msg) {
    void* c = e.c;
    auto G = [&](uint32_t va) { return e.rd(c, va); };
    auto S = [&](uint32_t va, uint32_t v) { e.wr(c, va, v); };
    auto K = [&](uint32_t rva, uint32_t a = 0, uint32_t cx = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0, uint32_t di = 0,
                 uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0, uint32_t s5 = 0, uint32_t s6 = 0) {
        return e.h(c, rva, a, cx, d, b, s, di, s0, s1, s2, s3, s4, s5, s6);
    };
    auto M = [&](uint32_t o, uint32_t v) { std::memcpy(msg + o, &v, 4); };
    const uint32_t mp = (uint32_t)(uintptr_t)msg;
    const uint32_t p0 = K(0x97CD0);
    if (p0 == 0) return;
    M(0x18, 0);
    K(0x16300, 0, 0, 0, 0, 0, 0, mp);
    S(0x6FBCBEF0, 0);
    int32_t x = (int32_t)G(0x6FBCB828);
    if (x < 0 || x > (int32_t)G(0x6FB8BC48) / 2) return;
    int32_t y = (int32_t)G(0x6FBCB824);
    const int32_t hgt = (int32_t)G(0x6FB8BC4C);
    if (y < 0 || y > hgt - 0x31) return;
    const int32_t bx = (int32_t)G(0x6FBCB9A0);
    bool inBtn = x >= bx + 0x110 && x <= bx + 0x130;
    if (inBtn) { const int32_t by = (int32_t)G(0x6FBCB9A4); inBtn = y >= by + hgt - 0x5F && y <= by + hgt - 0x3F; }
    if (inBtn) {
        M(0x18, 1); M(0x1C, 0);
        const uint32_t saved = G(0x6FBAAE10);
        if (K(0xBF8C) == 0 || G(0x6FBC9854) == 0) return;
        if (G(0x6FB8D5D0) != 0x26) {
            const uint32_t t = K(0xBF92, 0, 0, 0, 0, 0, 0, 0x139B);
            K(0xBF80, 0, 0, 0, 0, 0, 0, 0x6FB8359F, t);
            K(0x3664, 0, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
        }
        uint32_t edx = G(0x6FBAAE10);
        if (edx == 0) {
            if (G(0x6FBC3A68) != 0) return;
            uint32_t i = 0;
            do {
                if (G(0x6FBAAD80 + 4 * i) == 1) {
                    const uint32_t kind = G(G(0x6FB8D538 + 4 * i) + 0x90);
                    if (kind > 4 || kind == 2 || kind == 3) return;
                    if (kind == 1) { K(0xC2790, 0, i, 1, 0, 0, 0, 0); edx = G(0x6FBAAE10); }
                    else if (kind == 4 && G(0x6FBC9721) != 0) { K(0x47B20, 0, 0, 0, 0, 0, G(0x6FBC971D)); edx = G(0x6FBAAE10); }
                }
                ++i;
            } while ((int32_t)i < (int32_t)G(0x6FB8D5D0));
        }
        const uint32_t unit = G(0x6FBCBBFC);
        if (unit == 0) { edx = edx == 0; S(0x6FBAAE10, edx); }
        else if (G(unit) == 0 && K(0xA5440, unit, 0, edx) == 0) { edx = edx == 0; S(0x6FBAAE10, edx); }
        const uint32_t on = edx;
        if (saved == 0) {
            if (on) { const uint32_t t = K(0xD0B4, 0, 0, 0, 0, 0, 0, 0x6FB8099C); K(0x3CCC0, 3, 0, 0, 0, 0, 0, t, 0x6FB8099C); }
        } else if (!on) {
            const uint32_t t = K(0xD0B4, 0, 0, 0, 0, 0, 0, 0x6FB8099C); K(0x3CC60, 3, 0, 0, 0, 0, 0, t, 0x6FB8099C); K(0x8BBF0);
        }
        if (G(0x6FBAAD84) == 0 && G(0x6FBAAD90) == 0) {
            if (on) {
                const int32_t v = (int32_t)G(0x6FB8BC48) / 4;
                S(0x6FBCC418, (uint32_t)v); S(0x6FBCC414, 2);
                K(0xD1B0, 0, (uint32_t)v);
                const int32_t a = (int32_t)G(0x6FBA9E14), qq = a / 4;
                K(0x8A4D0, G(0x6FBA9E18), (uint32_t)(a + qq), 0, 0, G(0x6FBCC3C4), 0, (uint32_t)qq);
            } else {
                S(0x6FBCC418, 0); S(0x6FBCC414, 0);
                K(0xD1B0, 0, 0);
                K(0x8A4D0, G(0x6FBA9E18), G(0x6FBA9E14), 0, 0, G(0x6FBCC3C4), 0, 0);
            }
        } else if (on) {
            S(0x6FBCC418, 0); S(0x6FBCC414, 3);
            K(0xD1B0, 0, 0);
            K(0x8A4D0, G(0x6FBA9E18), G(0x6FBA9E14), 0, 0, G(0x6FBCC3C4), 0, 0);
            S(0x6FBCC3DC, 0); S(0x6FBCC3E0, 0); S(0x6FBCC3F8, 0); S(0x6FBCC3FC, 0);
        } else {
            const int32_t v = -((int32_t)G(0x6FB8BC48) / 4);
            S(0x6FBCC418, (uint32_t)v); S(0x6FBCC414, 1);
            K(0xD1B0, 0, (uint32_t)v);
            const int32_t a = (int32_t)G(0x6FBA9E14), qq = a / 4;
            K(0x8A4D0, G(0x6FBA9E18), (uint32_t)(a - qq), 0, 0, G(0x6FBCC3C4), 0, (uint32_t)-qq);
        }
        return;
    }
    const uint32_t o = K(0xC202, 0, 0, 0, 0, 0, 0, G(G(0x6FBCBBFC) + 0x60));
    bool ended = false;
    for (uint32_t i = 0; i < 11 && !ended; ++i) {
        const uint32_t r = 0x6FBCBD80u + i * 0x14;
        const int32_t l = (int32_t)G(r);
        if (l <= 0) continue;
        x = (int32_t)G(0x6FBCB828);
        if (x < l || x > (int32_t)G(r + 4)) continue;
        y = (int32_t)G(0x6FBCB824);
        if (y < (int32_t)G(r + 8) || y > (int32_t)G(r + 0xC)) continue;
        if (o == 0) {
            const uint32_t w = G(p0 + 0x60);
            if (w == 0) continue;
            const uint32_t r2 = K(0xC262, 0, 0, 0, 0, 0, 0, w, i);
            if (r2 == 0) continue;
            const uint32_t t = K(0x81CE0, r2);
            K(0x14A10, (t & 0xFFFFFF00u) | 0x61, i);
            S(0x6FBCBC28, 0); S(0x6FBCBC2C, 0); S(0x6FBCBC38, 0);
            continue;
        }
        uint32_t local = 0;
        const uint32_t lp = (uint32_t)(uintptr_t)&local;
        if (K(0xC826, 0, 0, 0, 0, 0, 0, o, i) && K(0xC898, 0, 0, 0, 0, 0, 0, o, p0, 0, 0, 0, 0) && K(0x8C760, 0, 0, 0, o, 0, lp, p0)) {
            K(0x81CE0, o);
            const uint32_t r3 = K(0x81E40, 0, 0, 0, 0, o, 0);
            const uint32_t t = K(0x88A70, 0, 0, 0, r3);
            K(0x14A10, (t & 0xFFFFFF00u) | 0x61, 0);
        } else if (K(0xC826, 0, 0, 0, 0, 0, 0, o, i) && K(0x8C760, 0, 0, 0, o, 0, lp, p0) && local != 0) {
            K(0x23F20, 0, G(p0 + 0xC), 0, 0, 0, 0, 0x56);
        } else {
            K(0x23F20, 0, G(p0 + 0xC), 0, 0, 0, 0, 0x55);
        }
        ended = true;
    }
    if (!ended) { S(0x6FBCC3DC, 0); S(0x6FBCC3E0, 0); S(0x6FBCC3F8, 0); S(0x6FBCC3FC, 0); }
    M(0x18, 1); M(0x1C, 0);
    K(0xBE84, 0, 0, 0, 0, 0, 0, mp);
}
