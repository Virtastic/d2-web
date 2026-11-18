// 1.13c D2Client+0x70C40: stdcall, one key message of the chat / text-entry line. Edit buffer 0x11EC80 (UTF-16, length via Unicode::strlen),
// cursor-less length 0x11C028, enable 0x11C02C, filter flag 0x11C038. Key (dword at +8): 8 = erase the last character; 0xD = skip leading whitespace
// and send the line through 0x6EE40 / 0x707F0 (or, when empty, close the entry); 0x1B = close the entry; anything else = append when the width
// list from Storm 10059 (thunk 0xD378) still fits. Closing (0xBF8C, count check, 0xFAD94 cleared, panel flags, 0xF51EC / 0xF51BC / 0xD0780 strings).
// Unicode::strlen / isWhitespace / Unicode(ushort) are D2Lang imports computed natively here (ASCII whitespace set 9..13, 0x20).
#include <cstdint>
#include "cl_env.h"
namespace { constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; } constexpr uint32_t kT = 0xB2000000u; }
extern "C" void d2c_70c40(uint32_t m, const cl::Env& e) {
    if ((int32_t)e.R(G(0x11C02C)) < 1) return;
    const uint32_t buf = G(0x11EC80);
    auto r16 = [&](uint32_t a) { return (e.R(a & ~3u) >> ((a & 2) * 8)) & 0xFFFFu; };
    auto w16 = [&](uint32_t a, uint32_t v) { const uint32_t al = a & ~3u, sh = (a & 2) * 8; e.W(al, (e.R(al) & ~(0xFFFFu << sh)) | ((v & 0xFFFFu) << sh)); };
    auto slen = [&](uint32_t p) { uint32_t n = 0; while (r16(p + 2 * n)) ++n; return n; };
    auto isws = [](uint32_t c) { return c == 0x20 || (c >= 9 && c <= 13); };
    const int32_t len = (int32_t)slen(buf);
    const uint32_t key = e.R(m + 8);
    auto handled = [&] { e.W(m + 0x18, 1); e.W(m + 0x1C, 0); e.H(0xBE84, m); };
    auto fatal = [&] { const uint32_t s = e.H(0xBF92, 0x139B); e.H(0xBF80, kT | 0xD359F, s); e.H(0x3664, 0xFFFFFFFFu); };
    // 0x1B / empty 0xD: leave the entry
    auto leave = [&]() -> bool {
        const uint32_t old = e.R(G(0xFAD94));
        e.H(0xBF8C);
        if (e.R(G(0xDD5D0)) != 0x26) { fatal(); return false; }
        e.W(G(0xFAD94), 0);
        if (old) {
            e.H(0x6F4A0);
            static const uint32_t fl[9] = { 0xFADB0, 0xFADDC, 0xFADE4, 0xFADE8, 0xFADEC, 0xFADF0, 0xFADF4, 0xFADF8, 0xFAE00 };
            bool any = false;
            for (uint32_t f : fl) any |= e.R(G(f)) != 0;
            if (!any) {
                e.W(G(0x11C434), 1);
                if (e.R(G(0x12349C)) == 0) {
                    e.W(G(0x12349C), 1);
                    if (e.R(G(0x123498)) != 0) { const uint32_t hw = e.H(0xD0B4); e.H(0x3CC60, hw, kT | 0xF51EC, 0, 0, 0, 2); e.W(G(0x123498), 0); }
                    const uint32_t hw = e.H(0xD0B4); e.H(0x3CCC0, hw, kT | 0xF51BC, 0, 0, 0, 4);
                }
            }
            if (e.R(G(0xFADA8)) != 0) { const uint32_t hw = e.H(0xD0B4); e.H(0x3CCC0, hw, kT | 0xD0780, 0, 0, 0, 5); }
        }
        return true;
    };
    if (key == 8) {
        const uint32_t n = e.R(G(0x11C028));
        if (n != 0) { e.W(G(0x11C028), n - 1); w16(buf + 2 * (n - 1), 0); }
    } else if (key == 0xD) {
        uint32_t skip = 0;
        while (isws(r16(buf + 2 * skip))) ++skip;
        const uint32_t p = buf + 2 * skip;
        if (slen(p) != 0) {
            e.H(0x6EE40, 0, 0, 0, 0, 0, 0, 0, p);
            char nb[0x100];
            uint32_t i = 0;
            for (; i < 0xFF && r16(buf + 2 * i); ++i) nb[i] = (char)r16(buf + 2 * i);
            nb[i] = 0;
            e.H(0x707F0, skip, 0, 0, 0, 0, buf, 0, (uint32_t)(uintptr_t)nb);
        } else {
            if (!leave()) return;
        }
    } else if (key == 0x1B) {
        if (!leave()) return;
    } else {
        if (e.R8(G(0x11C038)) == 0 && e.H(0xD3F0, 0, 0, 0, 0, 0, 0, key & 0xFF) == 0) { handled(); return; }
        if (e.R(G(0x11C028)) >= 0xFF || len >= 0xFF) { handled(); return; }
        const uint32_t ctx = e.H(0xD3A8, 0, 0, 0, 0, 0, 0, 0xD);
        const uint32_t w = e.R(G(0xDBC48));
        auto measure = [&](uint32_t& out) -> int32_t {
            const uint32_t head = e.H(0xD378, w - 0x109, 0, 0, 0, 0, 0, buf, (uint32_t)(uintptr_t)&out);
            int32_t last = 0;
            for (uint32_t nd = head; nd;) { const uint32_t item = e.R(nd), nx = e.R(nd + 4); last = (int32_t)e.H(0xD35A, 0, 0, 0, 0, 0, 0, item); nd = nx; }
            e.H(0xD3F6, 0, 0, 0, 0, 0, 0, head);
            return last;
        };
        uint32_t out = 0;
        const int32_t w1 = measure(out);
        if (out <= 3 && (out != 3 || !(w1 > (int32_t)(w - 0x113)))) {
            w16(buf + 2 * (uint32_t)len, e.R(m + 8) & 0xFFFF);
            const int32_t w2 = measure(out);
            if (out != 3 || w2 < (int32_t)(w - 0x113)) e.W(G(0x11C028), e.R(G(0x11C028)) + 1);
        }
        e.H(0xD3A8, 0, 0, 0, 0, 0, 0, ctx);
    }
    handled();
}
