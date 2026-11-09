// 1.13c D2Client+0x98DC0: left-button-down on the game view (stack arg = the input message; x = word +0xC, y = word +0xE; ret 4).
// Env-seam style: `h` calls a retail callee at its RVA with the registers (eax ecx edx ebx esi edi)
// and stack words its call site loads; `rd`/`wr` reach memory by virtual address (module globals at D2Client base 0x6FAB0000).
#include <cstdint>
#include <cstring>
struct D2c98dc0Env {
    void* c;
    uint32_t (*rd)(void* c, uint32_t va);
    void (*wr)(void* c, uint32_t va, uint32_t v);
    uint32_t (*h)(void* c, uint32_t rva, uint32_t a, uint32_t cx, uint32_t d, uint32_t b, uint32_t s, uint32_t di,
                  uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6);
};
// 1.13c D2Client+0x98DC0
extern "C" void d2c_98dc0(const D2c98dc0Env& e, uint8_t* msg) {
    void* c = e.c;
    auto G = [&](uint32_t va) { return e.rd(c, va); };
    auto S = [&](uint32_t va, uint32_t v) { e.wr(c, va, v); };
    auto K = [&](uint32_t rva, uint32_t a = 0, uint32_t cx = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0, uint32_t di = 0,
                 uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0, uint32_t s5 = 0, uint32_t s6 = 0) {
        return e.h(c, rva, a, cx, d, b, s, di, s0, s1, s2, s3, s4, s5, s6);
    };
    auto M = [&](uint32_t o, uint32_t v) { std::memcpy(msg + o, &v, 4); };
    auto R = [&](uint32_t o) { uint32_t v; std::memcpy(&v, msg + o, 4); return v; };
    const uint32_t mp = (uint32_t)(uintptr_t)msg;
    uint32_t unit = G(0x6FBCBBFC);
    if (G(0x6FBCC3A4) != 0 || unit == 0 || G(unit + 0x10) == 0x11) { M(0x18, 0); return; }
    uint16_t w;
    std::memcpy(&w, msg + 0xC, 2); const int32_t x = w;
    std::memcpy(&w, msg + 0xE, 2); const int32_t y = w;
    if (x > (int32_t)G(0x6FB8BC48)) { M(0x18, 1); M(0x1C, 0); K(0xBE84, 0, 0, 0, 0, 0, 0, mp); unit = G(0x6FBCBBFC); }
    K(0x908C0);
    const uint32_t u60 = G(unit + 0x60);
    uint32_t hit = 0;
    auto done88 = [&]() { M(0x18, 1); M(0x1C, 0); K(0x88A70, 0, 0, 0, 4); K(0xBE84, 0, 0, 0, 0, 0, 0, mp); };
    auto k96 = [&](uint32_t ptr, uint32_t idx) { return K(0x96AA0, 0, 0, 0, unit, 0, 0, unit, u60, (uint32_t)x, (uint32_t)y, R(8), ptr, idx); };
    auto k8f8 = [&]() { K(0x8F880, (uint32_t)y, 0, 0, 0, mp, (uint32_t)x, (uint32_t)y); };
    auto clr = [&]() { S(0x6FBCBC28, 0); S(0x6FBCBC2C, 0); S(0x6FBCBC38, 0); S(0x6FBCBC3C, 0); };
    if (G(0x6FBCC474) == 0) {
        if (K(0x8D3D0, (uint32_t)y)) {
            const uint32_t s = G(0x6FBCBC34);
            if (s == 0 || s == 0xB || s == 0xC || s == 0xF) { S(0x6FBCBE7C, 1); K(0x88A70, 0, 0, 0, 4); }
        }
        if (K(0x8BD90, 0, (uint32_t)x, (uint32_t)y)) {
            const uint32_t s = G(0x6FBCBC34);
            if (s == 0 || s == 0x13) { S(0x6FBCBEDC, 1); done88(); return; }
        }
    }
    if (K(0x26320, mp) == 0 && K(0x8C580, (uint32_t)x, (uint32_t)y) != 0) {
        if (K(0x8BD90, 0, (uint32_t)x, (uint32_t)y)) {
            const uint32_t open = G(0x6FBAAD84);
            S(0x6FBCBEDC, 1);
            K(0xBF8C);
            if (G(0x6FB8D5D0) != 0x26) { const uint32_t t = K(0xBF92, 0, 0, 0, 0, 0, 0, 0x139B); K(0xBF80, 0, 0, 0, 0, 0, 0, 0x6FB8359F, t); K(0x3664, 0, 0, 0, 0, 0, 0, 0xFFFFFFFFu); }
            S(0x6FBAAD84, 0);
            if (open) { K(0xBF720); const uint32_t t = K(0xD0B4, 0, 0, 0, 0, 0, 0, 0x6FB805B0); K(0x3CC60, 4, 0, 0, 0, 0, 0, t, 0x6FB805B0); K(0xBE2E0); }
            K(0x8BBF0);
            if (!G(0x6FBAAD88) && !G(0x6FBAADD0) && !G(0x6FBAADBC) && !G(0x6FBAADC0) && !G(0x6FBAAE10) && !G(0x6FBAAE14) && !G(0x6FBAADD8)) {
                S(0x6FBCC418, 0); S(0x6FBCC414, 0);
                K(0xD1B0, 0, 0);
                K(0x8A4D0, G(0x6FBA9E18), G(0x6FBA9E14), 0, 0, G(0x6FBCC3C4), 0, 0);
            } else {
                const int32_t v = (int32_t)G(0x6FB8BC48) / 4;
                S(0x6FBCC418, (uint32_t)v); S(0x6FBCC414, 2);
                K(0xD1B0, 0, (uint32_t)v);
                const int32_t a14 = (int32_t)G(0x6FBA9E14), q = a14 / 4;
                K(0x8A4D0, G(0x6FBA9E18), (uint32_t)(a14 + q), 0, 0, G(0x6FBCC3C4), 0, (uint32_t)q);
            }
            done88();
            return;
        }
        M(0x18, 1); M(0x1C, 0);
        uint32_t res = 0;
        bool chk9854 = false;
        if (x >= (int32_t)G(0x6FBBB3E4) && x <= (int32_t)G(0x6FBBB3E8) && y >= (int32_t)G(0x6FBBB3EC) && y <= (int32_t)G(0x6FBBB3F0)) {
            res = k96(0x6FBBB3E0, 0); hit = 1;
        } else {
            uint32_t i = 0;
            for (; i < 11; ++i) {
                const uint32_t r = 0x6FBCBCA0u + i * 0x14;
                if (x >= (int32_t)G(r) && x <= (int32_t)G(r + 4) && y >= (int32_t)G(r + 8) && y <= (int32_t)G(r + 0xC)) break;
            }
            if (i == 11) chk9854 = true;
            else {
                const uint32_t o = K(0xC202, 0, 0, 0, 0, 0, 0, u60);
                const uint32_t m = G(0x6FBCB864);
                bool table = m == 6 || m == 8;
                if (!table) {
                    const uint32_t o2 = K(0xC262, 0, 0, 0, 0, 0, 0, u60, i);
                    if (!K(0x8C2B0, o2, o)) table = true;
                    else {
                        if (K(0x81CB0, o) == 0 && K(0x923F0) != 0) {
                            K(0x14910, (o2 & 0xFFFFFF00u) | 0x28, o ? G(o + 0xC) : 0xFFFFFFFFu, o2 ? G(o2 + 0xC) : 0xFFFFFFFFu);
                            K(0x81CE0, o);
                            const uint32_t r2 = K(0x81E40, 0, 0, 0, 0, o2, 0);
                            K(0x88A70, 0, 0, 0, r2);
                            hit = 1;
                        }
                        res = 0;
                        if (!hit) chk9854 = true;
                    }
                }
                if (table) {
                    const uint32_t fn = G(0x6FB90EC4u + i * 4);
                    res = K(fn - 0x6FAB0000u, 0, unit, u60, 0, 0, 0, i);
                    hit = 1;
                }
            }
        }
        if (chk9854 && G(0x6FBC9854) && K(0x8C150, (uint32_t)y, 0, (uint32_t)x)) {
            M(0x18, 1); K(0x90F60); K(0xBE84, 0, 0, 0, 0, 0, 0, mp);
            return;
        }
        M(0x1C, 0); M(0x18, 1);
        if (res == 0) {
            const uint32_t m = G(0x6FBCB864);
            if (m == 6) K(0xBEB90);
            else if (m == 8) K(0x14940, (m & 0xFFFFFF00u) | 0x4C, 0xFFFFFFFFu);
        }
        K(0xBE84, 0, 0, 0, 0, 0, 0, mp);
        clr();
        if (hit) return;
        k8f8();
        return;
    }
    const uint32_t s = G(0x6FBCBC34);
    uint32_t l = 0, r = 0, t = 0, b = 0, ptr = 0, idx = 0;
    bool known = true;
    if (s == 0xB) { l = 0x6FBBB28C; ptr = 0x6FBBB288; idx = 2; }
    else if (s == 0xC) { const bool c9 = G(0x6FBC9854) != 0; l = c9 ? 0x6FBBB2D4 : 0x6FBBB39C; ptr = c9 ? 0x6FBBB2D0 : 0x6FBBB398; idx = 4; }
    else if (s == 0xE) { l = 0x6FBBB3CC; ptr = 0x6FBBB3C8; idx = 3; }
    else known = false;
    if (known) {
        r = l + 4; t = l + 8; b = l + 0xC;
        if (x >= (int32_t)G(l) && x <= (int32_t)G(r) && y >= (int32_t)G(t) && y <= (int32_t)G(b)) {
            const uint32_t v = k96(ptr, idx);
            M(0x1C, 0); M(0x18, 1); clr();
            K(0xBE84, 0, 0, 0, 0, 0, 0, mp);
            if (s == 0xB && v) K(0x595C0);
            return;
        }
    }
    M(0x18, 0);
    k8f8();
}
