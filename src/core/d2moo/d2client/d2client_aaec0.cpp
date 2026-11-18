// 1.13c D2Client+0xAAEC0: draw the trade panel (backdrop, the action buttons, the buy/sell button, the tabs and their labels,
// the hover tip and the footer). void, no arguments. Written from the disassembly.
// Module globals: 0x11BBFC player unit (+0x10 mode), 0xFADD0 pending flag, 0xDD5D0 (must be 0x26), 0xFAD84/0xFAD90, 0x11C414/0x11C418
// view shift, 0xDBC48/0xDBC4C, 0xF9E14/0xF9E18, 0x11C3C4, 0x11B9A0/0x11B9A4 panel origin, 0x11B824/0x11B828 cursor, 0x11B9C4 cached
// buy/sell cel file, 0xFCDC1/0xFCDC5/0xFCDC9 cel files, 0x119854, 0x11973B, 0xFCDD6 pressed button, 0xFCDBC byte, 0xFCDBD hot tab,
// 0xFCDDA tab count, 0xFCD8C tab table (5 bytes: id, shown flag), 0xDE6D8 tab layout (0x18 bytes: x, y, text x, text y), 0x11C0A0,
// 0x115591 byte, 0xFCDDE footer switch.
#include <cstdint>
#include "cl_env.h"

namespace {
constexpr uint32_t kBase = 0x6FAB0000u;
constexpr uint32_t G(uint32_t rva) { return kBase + rva; }
int32_t half(int32_t v) { return v / 2; }   // cdq; sub eax,edx; sar 1
}

extern "C" void d2c_aaec0(const cl::Env& e) {
    // retail's frame: the 0x48-byte cel request handed to 0xD096 by address, and the wsprintf buffer
    alignas(4) volatile uint32_t st[18] = {};
    alignas(4) volatile uint8_t buf[0xF0] = {};
    const uint32_t ps = (uint32_t)(uintptr_t)st;
    auto cel = [&](uint32_t x, uint32_t y) {
        e.H(0xD096, ps, x, y, 0xFFFFFFFFu, 5);
        if (e.blk) e.blk(e.ctx, 0xD096, ps, 18);
    };
    auto X0 = [&] { return e.R(G(0x11B9A0)); };
    auto Y0 = [&] { return e.R(G(0x11B9A4)); };
    auto exitPath = [&](uint32_t id) {   // retail exits here; the capture's recorder returns and the code runs on
        const uint32_t s = e.H(0xBF92, id);
        e.H(0xBF80, G(0xD359F), s);
        e.H(0x3664, 0xFFFFFFFFu);
    };
    auto text = [&](uint32_t s, uint32_t x, uint32_t y, uint32_t color) { e.H(0xD37E, y, color, 0, 0, 0, 0, s, x); };

    e.H(0xAA3E0);
    const uint32_t pl = e.R(G(0x11BBFC));
    if (!pl || e.R(pl + 0x10) == 0x11) {
        const uint32_t pend = e.R(G(0xFADD0));
        e.H(0xBF8C);
        if (e.R(G(0xDD5D0)) != 0x26) exitPath(0x139B);
        e.W(G(0xFADD0), 0);
        if (pend) {
            const uint32_t s = e.H(0xD0B4, G(0xD0828));
            e.H(0x3CC60, s, 0, 0, 0, 0, 5);
            e.H(0xAA380);
            e.H(0xBE2E0);
        }
        uint32_t cx, push;
        if (!e.R(G(0xFAD84)) && !e.R(G(0xFAD90))) {
            e.W(G(0x11C418), 0);
            e.W(G(0x11C414), 0);
            e.H(0xD1B0, 0, 0, 0, 0, 0, 0, 0);
            cx = e.R(G(0xF9E14));
            push = 0;
        } else {
            const uint32_t q = 0u - (uint32_t)((int32_t)e.R(G(0xDBC48)) / 4);
            e.W(G(0x11C418), q);
            e.W(G(0x11C414), 1);
            e.H(0xD1B0, 0, 0, 0, 0, 0, 0, q);
            const uint32_t c = e.R(G(0xF9E14));
            const uint32_t r = (uint32_t)((int32_t)c / 4);
            cx = c - r;
            push = 0u - r;
        }
        e.H(0x8A4D0, push, 0, 0, 0, 0, e.R(G(0xF9E18)), cx, 0, 0, e.R(G(0x11C3C4)));
        e.H(0xAA380);
    }
    e.H(0xD3A8, 0, 0, 0, 0, 0, 0, 1);
    // backdrop
    st[0xD] = e.R(G(0xFCDC1));
    st[0] = 0; st[0x10] = 0;
    cel(X0(), Y0() + e.R(G(0xDBC4C)) - 0xE0);
    st[0] = 1;
    cel(X0() + 0x100, Y0() + e.R(G(0xDBC4C)) - 0xE0);
    st[0] = 2;
    cel(X0(), Y0() + e.R(G(0xDBC4C)) - 0x30);
    st[0] = 3;
    cel(X0() + 0x100, Y0() + e.R(G(0xDBC4C)) - 0x30);
    const int32_t my = (int32_t)(e.R(G(0x11B828)) - X0());
    const int32_t mx = (int32_t)(e.R(G(0x11B824)) + Y0());
    st[0xD] = e.R(G(0xFCDC5));
    // action buttons
    uint32_t xs[5], n, dy;
    if (e.H(0xBF8C) && e.R(G(0x119854))) { dy = 1; n = 5; xs[0] = 5; xs[1] = 0x43; xs[2] = 0x81; xs[3] = 0xBF; xs[4] = 0xFD; }
    else { dy = 0; n = 4; xs[0] = 3; xs[1] = 0x51; xs[2] = 0x9F; xs[3] = 0xED; }
    for (uint32_t i = 0; i < n; i++) {
        if (i == e.R(G(0xFCDD6))) { st[0] = 2 * i; cel(xs[i] + X0(), dy - Y0() + 0x21); continue; }
        bool on = true;
        if (i >= 1 && i <= 4) {
            static const uint32_t kArg[4] = { 7, 0xF, 0x17, 0x1A };
            const uint32_t r = e.H(0xC58C, e.R(G(0x11973B)), kArg[i - 1], 0);
            if (i < 4) on = r != 0;
            else on = e.H(0xBF8C) && e.R(G(0x119854)) && r != 0;
        }
        if (on) { st[0] = 2 * i + 1; cel(xs[i] + X0(), dy - Y0() + 0x21); }
    }
    // buy/sell button
    uint32_t bs = e.R(G(0x11B9C4));
    if (!bs) {
        const uint32_t pb = (uint32_t)(uintptr_t)buf;
        e.H(0xCF114, pb, G(0xD0564), G(0xD357C));   // user32 wsprintfA (cdecl)
        bs = e.H(0x2B420, 0, 0, 0, 0, 0, pb);
        e.W(G(0x11B9C4), bs);
    }
    st[0xD] = bs;
    st[0] = (e.R8(G(0xFCDBC)) != 0) + 10;
    cel(X0() + 0x111, 0x1A1 - Y0());
    // tabs
    const uint32_t t = e.H(0xC1DE, e.H(0xC1F0, e.R(G(0x11BBFC))));
    st[0xD] = e.R(G(0xFCDC9));
    for (int32_t k = 0; k < (int32_t)e.R(G(0xFCDDA)); k++) {
        const uint32_t p = G(0xFCD8C) + 5 * (uint32_t)k, q = G(0xDE6D8) + 0x18 * (uint32_t)k;
        uint32_t color;
        if (e.R8(p + 4)) {
            const uint32_t hot = e.R(G(0xFCDBD)) == (uint32_t)k;
            st[0] = (e.R(p) == t ? 0 : 3) + hot;
            cel(e.R(q) + X0(), e.R(q + 4) - Y0());
            color = 0;
        } else color = 5;
        if (!e.R8(G(0xFCDBC)) && e.R(G(0xFCDBD)) != 0xFFFFFFFFu && (uint32_t)k == e.R(G(0xFCDBD))) color = 3;
        if (e.R(p) == t) {
            color = 3;
            st[0] = 0;
            cel(e.R(q) + X0(), e.R(q + 4) - Y0());
        }
        const uint32_t id = e.R(p);
        const uint32_t r = e.H(0xC214, id);
        uint32_t s;
        if (!r) s = e.H(0xD1EC, 0, 0, 0, 0, 0, 0, 0x150D);
        else if (id == 8) {
            if (!e.R(G(0x11C0A0))) {   // falls into the footer's 0xF96 arm and returns
                exitPath(0x60);
                const uint32_t f = e.H(0xD1EC, 0, 0, 0, 0, 0, 0, 0xF96);
                const uint32_t w = e.H(0xD35A, 0, 0, 0, 0, 0, 0, f);
                text(f, X0() - (uint32_t)half((int32_t)w) + 0xA0, 0x30 - Y0(), 0);
                return;
            }
            s = e.H(0xD1EC, 0, 0, 0, 0, 0, 0, e.R8(G(0x115591)) ? 0x13B8 : 0x13B7);
        } else s = r + 0x16E;
        text(s, e.R(q + 8) + X0(), e.R(q + 0xC) - Y0(), color);
    }
    // hover tip over the buy/sell button
    if (my >= 0x111 && my <= 0x134 && mx >= 0x183 && mx <= 0x1A4) {
        const uint32_t s = e.H(0xD1EC, 0, 0, 0, 0, 0, 0, 0x1022);
        const uint32_t h = (uint32_t)half((int32_t)e.H(0xD35A, 0, 0, 0, 0, 0, 0, s));
        e.H(0xD0BA, X0() - h + 0x11F, 0x172 - Y0(), h + X0() + 0x126, 0x183 - Y0(), 0);
        text(s, X0() - h + 0x124, 0x181 - Y0(), 0);
    }
    // footer
    const uint32_t f = e.H(0xD1EC, 0, 0, 0, 0, 0, 0, e.R(G(0xFCDDE)) ? 0xF96 : 0xF97);
    const uint32_t w = e.H(0xD35A, 0, 0, 0, 0, 0, 0, f);
    text(f, X0() - (uint32_t)half((int32_t)w) + 0xA0, 0x30 - Y0(), 0);
}
