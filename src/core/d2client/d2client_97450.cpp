// 1.13c D2Client+0x97450: left-button-up on the skill-bar strip (stack arg = the input message; x = word +0xC, y = word +0xE; ret 4).
// Env-seam style: `h` is the call into a retail callee at an RVA with the register and stack words its call site loads; `rd`/`wr`/`rb` reach the
// module globals by virtual address (D2Client base 0x6FAB0000).
#include <cstdint>
#include <cstring>
struct D2c97450Env {
    void* c;
    uint32_t (*rd)(void* c, uint32_t va);
    void (*wr)(void* c, uint32_t va, uint32_t v);
    uint32_t (*rb)(void* c, uint32_t va);
    uint32_t (*h)(void* c, uint32_t rva, uint32_t a, uint32_t cx, uint32_t d, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5);
    void (*fatal)(void* c);
};
// 1.13c D2Client+0x97450
extern "C" void d2c_97450(const D2c97450Env& e, uint8_t* msg) {
    void* c = e.c;
    auto G = [&](uint32_t va) { return e.rd(c, va); };
    auto S = [&](uint32_t va, uint32_t v) { e.wr(c, va, v); };
    auto K = [&](uint32_t rva, uint32_t a = 0, uint32_t cx = 0, uint32_t d = 0, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0, uint32_t s5 = 0) {
        return e.h(c, rva, a, cx, d, s0, s1, s2, s3, s4, s5);
    };
    auto w16 = [&](uint32_t o) { uint16_t v; std::memcpy(&v, msg + o, 2); return (uint32_t)v; };
    auto w32 = [&](uint32_t o, uint32_t v) { std::memcpy(msg + o, &v, 4); };
    const uint32_t mp = (uint32_t)(uintptr_t)msg;
    const uint32_t x = w16(0xC), y = w16(0xE);
    S(0x6FBCBEDC, 0); S(0x6FBCBEE0, 0);
    if (G(0x6FBCB834) != 0) { w32(0x18, 0); return; }
    if (G(0x6FBCBEEC) == 1) {
        uint32_t ebx = 0;
        const int32_t sh = (int32_t)G(0x6FB8BC4C), t9a4 = (int32_t)G(0x6FBCB9A4);
        const bool inside = (int32_t)x >= (int32_t)G(0x6FBBB408) && (int32_t)x <= (int32_t)G(0x6FBBB40C) && (int32_t)y >= t9a4 + sh - 0x6D && (int32_t)y <= (int32_t)G(0x6FBBB414);
        bool toTail = false;   // true: the L6F6 tail (edi = msg, then the commit); false: fall to the 4762F cleanup
        if (inside) {
            const int32_t n = (int32_t)G(0x6FB911C8);
            if (n > 0) {
                int32_t mx = (int32_t)G(0x6FBBB2A4);
                for (int32_t i = 0; i < (int32_t)G(0x6FB911C8); ++i) {
                    const uint32_t eb = 0x6FBBB238u + (uint32_t)i * 0x14;   // esi - 0xA
                    if (G(eb) == 1) {
                        const int32_t eax = (int32_t)(G(0x6FBCB9A0) + G(0x6FB911D0 + (uint32_t)(i + (int32_t)G(0x6FB911C4) * 4) * 4));
                        const int32_t cy = (int32_t)G(0x6FBBB430), top = (int32_t)G(0x6FBCB9A4) + (int32_t)G(0x6FB8BC4C);
                        if (mx > eax && mx < eax + 0x2D && cy > top - 0x6D && cy < top - 0x41 && G(eb + 4) == 1) {
                            const uint32_t kind = G(eb + 10);
                            if (kind == 9) {
                                w32(0x18, 1); w32(0x1C, 0);
                                K(0xBE84, 0, 0, 0, mp);
                                S(0x6FBCBEEC, 0);
                                if (!K(0xC202, 0, 0, 0, G(G(0x6FBCBBFC) + 0x60))) {
                                    S(0x6FBCB834, 0); S(0x6FBCB864, 1); S(0x6FBCB854, 5); S(0x6FBCB858, 0);
                                }
                                K(0x48730);
                                S(0x6FBBB2A4, 0); S(0x6FBBB430, 0);
                                return;
                            }
                            S(0x6FBCBC34, kind);
                            if (e.rb(c, 0x6FBCBC50) >= 5) { e.fatal(c); return; }
                            const uint16_t ax = (uint16_t)(G(eb + 10 + 4) & 0xFFFF);
                            if (ax == 0xE || ax == 0xC) { K(0x8BBF0); ebx = 0; }
                            else {
                                ebx = 1;
                                if (ax == 0x12) {
                                    S(0x6FBCBC90, 1);
                                    K(0x47D60, x, 0, 0, G(0x6FBCBBFC), 1, y, 0, 1, 1);
                                    mx = (int32_t)G(0x6FBBB2A4);
                                    S(0x6FBCBC90, 0);
                                } else {
                                    S(0x6FBCB834, 0); S(0x6FBCB854, 6); S(0x6FBCB858, G(eb + 10 + 6)); S(0x6FBCB864, 7);
                                    S(0x6FBCBC88, (uint32_t)i); S(0x6FBCBC8C, ax);
                                }
                            }
                        }
                    }
                }
                if (ebx) toTail = true;
            }
        }
        if (!toTail) {
            if (K(0x8CF60)) { K(0xC202, 0, 0, 0, G(G(0x6FBCBBFC) + 0x60)); K(0x162D0); }
            const int32_t n2 = (int32_t)G(0x6FB911C8);
            if (n2 > 0) for (int32_t i = 0; i < n2; ++i) { const uint32_t p = 0x6FBBB23Cu + (uint32_t)i * 0x14; if (G(p) == 1) S(p, 0); }
        }
        w32(0x18, 1); w32(0x1C, 0);
        K(0xBE84, 0, 0, 0, mp);
        S(0x6FBCBEEC, 0);
    }
    const uint32_t esi = G(G(0x6FBCBBFC) + 0x60);
    if (K(0x26320, mp)) return;
    if (K(0xC202, 0, 0, 0, esi)) return;
    if (!K(0x8BD90, 0, x, y)) return;
    K(0xC202, 0, 0, 0, G(G(0x6FBCBBFC) + 0x60));
    K(0x162D0);
    K(0xC2790, 0, 0xC, 1, 0);
    K(0x48150);
}
