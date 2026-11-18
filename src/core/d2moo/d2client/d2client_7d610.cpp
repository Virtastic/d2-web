// 1.13c D2Client+0x7D610 (stdcall, 2 stack words, ret 8): word-wrap the UCS-2 `text` into a new 0x26-byte message node (up to six
// line strings, colour byte `col`, expiry GetTickCount + 10000) and append it to the second message list (head dword 0x113AAA, count
// WORD 0x113AAE, both unaligned), dropping the oldest node past two. Written from the disassembly.
// The Unicode imports (strlen, isLineBreak, toUtf, strncpy) are calls through their IAT slots (0xCEAB0, 0xCEAE4, 0xCEAE8, 0xCEAA8).
// Retail quirk kept: after a line of n chars the next start is n (not start + n) and the remaining count is total - n.
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t M = 0xB2000000u;
constexpr uint32_t kFile = M | 0xd4bdc;   // 0x6FB84BDC, the source-file string handed to the Fog alloc
}
struct D2C7D610G {
    void* ctx; uint32_t (*at)(void* ctx, uint32_t rva); uint32_t now;
};
extern "C" void d2c_7d610(uint32_t text, uint32_t col, const D2C7D610G& g, const cl::Env& e) {
    auto G = [&](uint32_t rva) { return g.at(g.ctx, rva); };
    auto R = [&](uint32_t rva) { return e.R(G(rva)); };
    auto W = [&](uint32_t rva, uint32_t v) { e.W(G(rva), v); };
    auto W16 = [&](uint32_t a, uint32_t v) { e.W(a, (e.R(a) & 0xffff0000u) | (v & 0xffff)); };
    const uint32_t font = e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 0xd);
    const uint32_t nd = e.H(0xbf7a, 0xd9b, 0, 0, 0, 0, 0, 0x26, kFile);
    for (uint32_t o = 0; o < 0x24; o += 4) e.W(nd + o, 0);
    W16(nd + 0x24, 0);
    e.W(nd + 0x22, 0);
    e.W(nd + 0x1e, g.now + 10000);
    const int32_t total = (int32_t)e.H(0xceab0, 0, 0, 0, 0, 0, 0, text);
    W16(nd + 0x18, 0);
    int32_t n = total, start = 0;
    for (;;) {
        if (n > 0) {
            const uint32_t p = text + 2 * (uint32_t)start;
            do {
                if ((int32_t)e.H(0xd3e4, 0, 0, 0, 0, 0, 0, p, (uint32_t)n) <= (int32_t)R(0xdbc48) - 0x46) break;
                n--;
            } while (n > 0);
        }
        if (start + n != total && n > 0) {
            const uint32_t p = text + 2 * (uint32_t)start;
            do {
                if (e.H(0xceae4, 0, 0, 0, 0, 0, 0, p, (uint32_t)(n + 1)) != 0) break;
                n--;
            } while (n > 0);
        }
        if (n == 0) break;
        const uint32_t p = text + 2 * (uint32_t)start;
        uint8_t utf[0x400];
        e.H(0xceae8, (uint32_t)n, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)utf, p);
        if ((e.R(nd + 0x18) & 0xffff) >= 6) break;
        const uint32_t sz = 2 * (uint32_t)n + 2;
        const uint32_t ln = e.H(0xbf7a, 0xdc5, 0, 0, 0, 0, 0, sz, kFile);
        const uint32_t k = e.R(nd + 0x18) & 0xffff;
        e.W(nd + 4 * k, ln);
        const uint32_t dst = e.R(nd + 4 * k);
        for (uint32_t o = 0; o + 4 <= sz; o += 4) e.W(dst + o, 0);
        if (sz & 2) W16(dst + (sz & ~3u), 0);   // sz is even: the tail is 0 or 2 bytes
        e.H(0xceaa8, (uint32_t)n, 0, 0, 0, 0, 0, dst, p);
        W16(nd + 0x18, (e.R(nd + 0x18) + 1) & 0xffff);
        const int32_t rest = total - n;
        start = n;
        n = rest;
    }
    e.W(nd + 0x1a, col & 0xff);
    const uint32_t gh = 0x113aaa, gc = 0x113aae;
    const uint32_t head = R(gh);
    if (head != 0) {
        uint32_t t = head, cnt = 0;
        while (e.R(t + 0x22) != 0) { t = e.R(t + 0x22); cnt++; }
        if ((cnt & 0xffff) != (R(gc) & 0xffff) - 1) {
            const uint32_t m = e.H(0xbf92, 0xddf); e.H(0xbf80, M | 0xd359f, m); e.H(0x3664, 0xffffffffu); return;
        }
        e.W(t + 0x22, nd);
    } else {
        W(gh, nd);
    }
    W16(G(gc), R(gc) + 1);
    e.H(0x7b9c0, 0, 0, 0, 0, 0, nd);
    if ((R(gc) & 0xffff) > 2) {
        const uint32_t old = R(gh);
        W(gh, e.R(old + 0x22));
        e.H(0x7b330);
        e.Edj(0x7b330, old);
        W16(G(gc), R(gc) - 1);
    }
    if (R(0xfade0) != 0 && R(0x113a74) != 0) {
        const uint32_t o = R(0x113a74);
        e.H(e.R(e.R(o) + 0x3c) & 0x00ffffffu, 0, 0, 0, 0, 0, 0, o);   // vtable +0x3C (not vectored: the descriptor keeps this gate off)
        e.H(0x7aea0);
    }
    e.H(0xd3a8, 0, 0, 0, 0, 0, 0, font);
    e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 6);
}
