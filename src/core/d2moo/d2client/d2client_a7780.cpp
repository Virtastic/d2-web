// 1.13c D2Client+0xA7780: create, or re-use, the client item unit an item packet describes.
// stdcall, six stack words, ret 0x18: p (item spec dwords), rect (16 bytes or 0), src9 (9 bytes or 0), w14, w26, flag.
// Written from the disassembly. Module globals: 0x11BBFC player unit, 0x11C4D4 client list head (node +4 type, +8 id, +0xC key,
// +0x24 dword, +0x28 word, +0x30 next), 0x10A808 unit hash[128] (chain +0xE4, id +0xC), 0x11C390 byte index; *sgptDataTables
// item rows +0xA78/+0xA80 (stride 0x1A8), item type rows +0xA90/+0xA98 (stride 0x134).
#include <cstdint>
#include "cl_env.h"

namespace {
constexpr uint32_t kBase = 0x6FAB0000u;
constexpr uint32_t G(uint32_t rva) { return kBase + rva; }
}

extern "C" uint32_t d2c_a7780(uint32_t p, uint32_t rect, uint32_t src9, uint32_t w14, uint32_t w26, uint32_t flag, const cl::Env& e) {
    // the local player's type-7 list node id, -1 without one
    uint32_t want = 0xFFFFFFFFu;
    const uint32_t pl = e.R(G(0x11BBFC));
    if (pl) {
        const uint32_t key = e.R(pl + 0xC);
        for (uint32_t n = e.R(G(0x11C4D4)); n; n = e.R(n + 0x30))
            if (e.R(n + 4) == 7 && e.R(n + 0xC) == key) { want = e.R(n + 8); break; }
    }
    const uint32_t id = e.R(p);
    if (id == want)
        for (uint32_t h = e.R(G(0x10A808) + 4 * (id & 0x7F)); h; h = e.R(h + 0xE4))
            if (e.R(h + 0xC) == id) {
                if (e.R(h) != 1) {   // fatal: Fog 10265(0x5A) -> Fog 10024(file, msg) -> exit(-1)
                    const uint32_t m = e.H(0xBF92, 0x5A);
                    e.H(0xBF80, G(0xD359F), m);
                    e.H(0x3664, 0xFFFFFFFFu);
                    return 0;
                }
                e.H(0x69700, 0, 0, 0, 0, 0, h);
                e.H(0x66950);
                e.H(0xC43C, h, e.R(p + 0x18));
                return h;
            }
    const int32_t idx = (int32_t)e.R(p + 4);
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xA80)) return 0;
    const uint32_t row = e.R(e.dt + 0xA78) + (uint32_t)idx * 0x1A8;
    const int32_t ti = e.R16s(row + 0x18);
    if (ti < 0 || ti >= (int32_t)e.R(e.dt + 0xA98)) return 0;
    const uint32_t trow = (uint32_t)ti * 0x134 + e.R(e.dt + 0xA90);
    if (!trow || !row) return 0;

    const uint32_t u = e.H(0xC69A, 0, 1);
    if (flag && u) e.W(u + 0xC4, e.R(u + 0xC4) | 0x600000);
    const uint32_t p2 = e.R(p + 8), p3 = e.R(p + 0xC);
    e.W(u, 1); e.W(u + 4, e.R(p + 4)); e.W(u + 0xC, e.R(p)); e.W(u + 8, 0); e.W(u + 0xD4, 0);
    const uint32_t rs = e.H(0xA5A60, 0, 0, 0, 0, 0, p3, p2, 0);
    e.H(0xC634, 0, 0, 0, 0, 0, 0, u + 0x20);
    if (rs) e.W(u + 0x28, e.Lcg(rs + 0x6C));
    e.H(0xC3B2, 0, 0, 0, 0, 0, 0, u + 0x20, e.R(u + 0x28));
    e.H(0x4C370, 0, 0, 0, 0, 0, u, 0, 0);
    auto data = [&]() -> uint32_t { return e.R(u) == 1 ? e.R(u + 0x14) : 0u; };
    auto w16 = [&](uint32_t a, uint32_t v) { e.W(a, (e.R(a) & 0xFFFF0000u) | (v & 0xFFFF)); };
    if (uint32_t d = data()) w16(d + 0x16, e.R(d + 0x16) | (e.R(p + 0x10) & 0xFFFF));
    if (uint32_t d = data(); d && (e.R8(d + 0x16) & 2)) w16(d + 0x26, w26);
    if (uint32_t d = data()) e.W(d + 0x3C, e.R(p + 0x14));
    const uint32_t name = e.H(0xD1EC, 0, 0, 0, 0, 0, 0, (e.R(p + 0x14) & 0xFFFF0000u) | (e.R(row + 6) & 0xFFFF));
    e.H(0xC22C, 0, 0, 0, 0, 0, 0, u, name);
    if (src9) {
        if (uint32_t d = data()) w16(d + 0x14, w14);
        uint32_t d = data(); d = d ? d + 0x1C : 0;
        for (uint32_t i = 0; i < 9; ++i) { const uint32_t a = d + i; e.W(a, (e.R(a) & ~0xFFu) | e.R8(src9 + i)); }
    }
    if (rect) if (uint32_t d = data(); d && d + 4)
        for (uint32_t i = 0; i < 16; i += 4) e.W(d + 4 + i, e.R(rect + i));
    e.H(0x4D160, rs, p2, p3, (uint32_t)idx, e.R(p + 0x18), u);
    e.H(0xC682, e.R(u + 0x2C), e.R8(trow + 0xE));
    e.H(0x66950);
    e.H(0xA70C0, 0, 0, 0, 0, 0, (uint32_t)idx, u);
    const uint32_t b4d = e.R8(row + 0x4D);
    uint32_t pal = b4d < 8 ? b4d + 2 : 2;
    if (uint32_t d = data(); d && (e.R8(d + 0x16) & 8) && !e.H(0x1510, 0, 0, 0, 0, 0, (uint32_t)idx, 0, 0xF))
        pal = e.H(0x539E0, 0, 0, 0, 0, 0, u, 0, w14);
    const uint32_t cls = e.R8(G(0x11C390));
    if (uint32_t r = e.H(0x1260, 0, 0, 0, 0, 0, (uint32_t)idx)) {
        const uint32_t b = e.R8(r + cls + 0x122);
        if (b) { pal = b; if (pal == 0xFF) pal = e.H(0x21B10, 0, 0, 0, 0, 0, u) ? 0 : 1; }
    }
    if (uint32_t d = data(); d && (e.R8(d + 0x16) & 2)) {
        const uint32_t x = e.H(0xC604, (uint32_t)e.R16s(d + 0x26));
        const uint32_t b = e.R8(x + cls + 0x28);
        if (b) pal = b;
    }
    if ((int32_t)pal >= 0x1E) pal = 2;
    uint32_t g = e.R(u + 0x54);
    if (!g) { const uint32_t m = e.H(0xBF92, 0xCE4); e.H(0xBF80, G(0xD359F), m); e.H(0x3664, 0xFFFFFFFFu); return 0; }
    e.W(g + 0x34, pal);
    g = e.R(u + 0x54);
    if (!g) { const uint32_t m = e.H(0xBF92, 0xCFD); e.H(0xBF80, G(0xD359F), m); e.H(0x3664, 0xFFFFFFFFu); return 0; }
    e.W(g + 0x38, pal);
    const uint32_t k = e.H(0xA7360, 0, 0, 0, 0, 0, u);
    if (const uint32_t head = e.R(G(0x11C4D4))) {
        const uint32_t key = e.R(k + 0xC);
        uint32_t n = head;
        while (n && e.R(n + 8) != key) n = e.R(n + 0x30);
        if (n && e.R(n + 4) == 7) {
            uint32_t m = head;
            while (m && e.R(m + 8) != key) m = e.R(m + 0x30);
            if (m && m + 0x24) {
                volatile uint32_t loc[2];   // word [m+0x28] then dword [m+0x24]; the upper half of loc[0] is never written by retail
                loc[0] = e.R(m + 0x28) & 0xFFFF; loc[1] = e.R(m + 0x24);
                e.H(0x4EF50, u, (uint32_t)(uintptr_t)loc);
            }
        }
    }
    if (e.H(0xC54A, u)) e.H(0xC63A, 0, u);
    e.H(0x23170, 0, 0, 0, 0, 0, u);
    return u;
}
