// 1.13c D2Client+0x7B730 (no arguments, plain ret): draw the overhead message list and expire its old nodes. Written from the
// disassembly. Node (0x26 bytes): six wide-string pointers at +0, colour at +0x1A,
// expiry tick at +0x1E, next at +0x22. Globals are reached through `at` (module RVA -> 32-bit address of the pinned bytes); the
// head is the dword at 0x113AA4 and the count the WORD at 0x113AA8. The wrap buffer is a stack local (400 UCS-2 chars), as in retail.
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t M = 0xB2000000u;   // a module address as the capture records it
constexpr uint32_t kFile = M | 0xd4bdc;   // 0x6FB84BDC, the source-file string handed to the Fog free
}
struct D2C7B730G {
    void* ctx; uint32_t (*at)(void* ctx, uint32_t rva); uint32_t now;
    void* wctx; void (*ws)(void* wctx, uint32_t rva, const char16_t* w);   // the wide string handed to 0xD378 in ECX, logged after its call
};
extern "C" void d2c_7b730(const D2C7B730G& g, const cl::Env& e) {
    auto G = [&](uint32_t rva) { return g.at(g.ctx, rva); };
    auto R = [&](uint32_t rva) { return e.R(G(rva)); };
    auto W = [&](uint32_t rva, uint32_t v) { e.W(G(rva), v); };
    auto ulen = [&](uint32_t s) { uint32_t n = 0; while (e.R(s + 2 * n) & 0xffff) n++; return n; };   // Unicode::strlen
    const uint32_t cnt = G(0x113aa8);
    if ((e.R(cnt) & 0xffff) == 0) return;
    const uint32_t font = e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 0xd);
    const int32_t y0 = R(0xfadcc) != 0 ? 0x5f : 0x14;
    int32_t x = 0xf, lines = 0;
    bool wrap = false;
    const uint32_t mode = R(0x11c414);
    if (mode == 1) wrap = true;
    else if (mode == 2) { x = (int32_t)R(0xdbc48) / 2 + 0xf; wrap = true; }
    auto rect = [&](int32_t a, int32_t b, int32_t c, int32_t d) {   // 0xD0BA: six stack words, stdcall
        const uint32_t s[6] = { (uint32_t)a, (uint32_t)b, (uint32_t)c, (uint32_t)d, 0, 1 };
        e.Xs(1);
        e.H(0xd0ba, s[0], s[1], s[2], s[3], 0);
        if (e.blk) e.blk(e.ctx, 0xd0ba, (uint32_t)(uintptr_t)s, 6);
    };
    for (uint32_t n = R(0x113aa4); n != 0; n = e.R(n + 0x22)) {
        for (uint32_t i = 0; i < 6; i++) {
            const uint32_t str = e.R(n + 4 * i);
            if (str == 0) continue;
            if (wrap) {
                char16_t buf[400];
                for (int k = 0; k < 400; k++) buf[k] = 0;                     // 400 Unicode default constructors
                for (uint32_t k = 0;; k++) { buf[k] = (char16_t)(e.R(str + 2 * k) & 0xffff); if (!buf[k]) break; }   // Unicode::strcpy
                uint32_t out = 0;
                uint32_t l = e.H(0xd378, 0x12c, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)buf, (uint32_t)(uintptr_t)&out);
                if (g.ws) g.ws(g.wctx, 0xd378, buf);
                if (l == 0) continue;
                int32_t y = lines * 0xf - 0xe + y0;
                do {
                    const uint32_t next = e.R(l + 4), t = e.R(l);
                    const int32_t w = (int32_t)e.H(0xd3e4, 0, 0, 0, 0, 0, 0, t, ulen(t));
                    if (w != 0) {
                        rect(x - 5, y, x - 5 + w + 0xa, y + 0x10);
                        e.H(0xd37e, (uint32_t)(y + 0xe), e.R(n + 0x1a), 0, 0, 0, 0, e.R(l), (uint32_t)x);
                        lines++; y += 0xf;
                    }
                    e.H(0xbf86, 0xa43, 0, 0, 0, 0, 0, e.R(l), kFile);
                    e.H(0xbf86, 0xa44, 0, 0, 0, 0, 0, l, kFile);
                    l = next;
                } while (l != 0);
            } else {
                const int32_t w = (int32_t)e.H(0xd3e4, 0, 0, 0, 0, 0, 0, str, ulen(str));
                if (w == 0) continue;
                const int32_t y = lines * 0xf + y0;
                rect(x - 4, y - 0xe, x - 4 + w + 8, y - 0xe + 0x10);
                e.H(0xd37e, (uint32_t)y, e.R(n + 0x1a), 0, 0, 0, 0, e.R(n + 4 * i), (uint32_t)x);
                lines++;
            }
        }
    }
    const uint32_t now = g.now;
    uint32_t prev = 0, n = R(0x113aa4);
    while (n != 0) {
        if (e.R(n + 0x1e) < now) {
            e.W(cnt, (e.R(cnt) & 0xffff0000u) | ((e.R(cnt) - 1) & 0xffff));   // dec word
            const uint32_t nx = e.R(n + 0x22);
            if (R(0x113aa4) == n) W(0x113aa4, nx);
            for (uint32_t i = 0; i < 6; i++) if (e.R(n + 4 * i)) e.H(0xbf86, 0x6af, 0, 0, 0, 0, 0, e.R(n + 4 * i), kFile);
            e.H(0xbf86, 0x6b2, 0, 0, 0, 0, 0, n, kFile);
            if (prev) e.W(prev + 0x22, nx);
            n = nx;
        } else { prev = n; n = e.R(n + 0x22); }
    }
    e.H(0xd3a8, 0, 0, 0, 0, 0, 0, font);
}
