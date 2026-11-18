// 1.13c D2Client+0x9BED0 -- store panel WM_LBUTTONDOWN handler (stdcall msg, ret 4; x = word at msg+0xC, y = word at
// msg+0xE, results at msg+0x18 / +0x1C). Panel mode 0x11BC34 outside 0..4: consumed. With the offer pointer 0x11B834
// clear the hit tests run: the tab strip (X0 = 0x11B9A0, four tabs of 0x50 / 0x50 / 0x50 / 0x50 wide, enable words at
// 0xE1186 / 0xE1198 / 0xE11AA / 0xE11BC, the current tab byte at 0x11BC48), the offer-tab strip (table 0x10B238, stride
// 0x14) and the item box (inventory item found through the id at 0x11971D in the chain table 0x10A808). With the offer
// pointer set a click inside the box drives the item routines. The assertion (line 0x5A) falls through into the item
// routine call with the reporter's return in place of the item, as retail's registers do.
#include "cl_tbl.h"
#include <cstring>
extern "C" uint32_t d2c_9bed0(uint32_t msg, const cl::Env& e) {
    using clt::G; using clt::S;
    const int32_t x = (int32_t)(e.R(msg + 0xC) & 0xffffu), y = (int32_t)(e.R(msg + 0xE) & 0xffffu);
    auto sg = [&](uint32_t rva) { return (int32_t)G(e, rva); };
    auto o511 = [&] { e.W(msg + 0x18, 1); e.W(msg + 0x1C, 0); e.H(0xBE84, msg); };
    auto c88a70 = [&](uint32_t ebx) { e.H(0x88A70, 0, 0, 0, 0, 0, 0, 0, 0, ebx); };
    auto lookup = [&]() -> uint32_t {
        const uint32_t id = G(e, 0x11971D);
        uint32_t p = G(e, 0x10A808 + (id & 0x7F) * 4);
        while (p) { if (e.R(p + 0xC) == id) return p; p = e.R(p + 0xE4); }
        return 0;
    };
    // the item-routine call both hit tests share: the chain item, its +0x60 word, x, y, msg+8, the tag, the tab byte
    auto itemCall = [&](uint32_t item, uint32_t yy) {
        e.Xs(0x6FBBB3B0u, G(e, 0x11BC48) & 0xffu);
        e.H(0x96AA0, item, e.R(item + 0x60), (uint32_t)x, yy, e.R(msg + 8));
    };
    auto fatal5a = [&]() -> uint32_t {
        const uint32_t r = e.H(0xBF92, 0x5A);
        e.H(0xBF80, 0x6FB8359Fu, r, 0x5A);
        return e.H(0x3664, 0xFFFFFFFFu);
    };
    const uint32_t mode = G(e, 0x11BC34);
    if (mode > 4) { o511(); return 0; }
    if (!e.H(0x26320, 0, 0, 0, 0, 0, msg) && e.H(0x8BD90, 0, 0, 0, 0, 0, 0, (uint32_t)x, (uint32_t)y) && e.H(0x8C3E0)) {
        S(e, 0x11BEDC, 1);
        e.W(msg + 0x18, 1); e.W(msg + 0x1C, 0);
        c88a70(4);
        e.H(0xBE84, msg);
        return 0;
    }
    if (G(e, 0x11B834) != 0) {
        if (e.H(0x26320, 0, 0, 0, 0, 0, msg) == 1) { e.H(0x8D300); return 0; }
        if (x > sg(0x10B2F8) || y > sg(0x10B304)) { e.H(0x98DC0, msg); return 0; }
        o511();
        if (x < sg(0x10B3B4) || x > sg(0x10B3B8) || y < sg(0x10B3BC) || y > sg(0x10B3C0) || G(e, 0x11B864) == 6 || !G(e, 0x119721)) return 0;
        const uint32_t p = lookup();
        if (!p) return 0;
        if (e.R(p) != 1) {   // retail goes on with EAX = the reporter's return and ESI still the item
            const uint32_t eax = fatal5a();
            itemCall(eax, p);
            o511();
            return 0;
        }
        uint32_t buf[32]; std::memset(buf, 0, sizeof buf);
        uint32_t local = 0;
        if (!e.H(0x46230, 1, (uint32_t)(uintptr_t)buf, 0x40, 0, 0, (uint32_t)(uintptr_t)&local, G(e, 0x11B834), (uint32_t)(uintptr_t)buf)) return 0;
        e.H(0x95F80, p, 1, (uint32_t)x, 0, 0, e.R(msg + 8), G(e, 0x11B834), (uint32_t)y, 0, 1);
        return 0;
    }
    if (e.H(0x26320, 0, 0, 0, 0, 0, msg) == 1) { e.H(0x8D300); return 0; }
    e.W(msg + 0x18, 0);
    const int32_t X0 = sg(0x11B9A0), Y0 = sg(0x11B9A4), Hh = sg(0xDBC4C), W = sg(0xDBC48);
    if (x >= X0 && x <= ((W - (W >> 31)) >> 1) - 1 && y >= Y0 + Hh - 0x1DF && y <= Y0 + Hh - 0x1C1) {
        int tab = -1;
        const uint32_t cur = G(e, 0x11BC48) & 0xffu;
        if (x <= X0 + 0x50) { if (G(e, 0xE1186) && cur != 0) tab = 0; }
        else if (x <= X0 + 0xA0) { if (G(e, 0xE1198) && cur != 1) tab = 1; }
        else if (x <= X0 + 0xF0) { if (G(e, 0xE11AA) && cur != 2) tab = 2; }
        else if (x < X0 + 0x140) { if (G(e, 0xE11BC) && cur != 3) tab = 3; }
        if (tab >= 0) { c88a70(6); S(e, 0x11BC4C, 0); e.H(0x8F710, (uint32_t)tab); }
        o511();
        return 0;
    }
    if (x < sg(0x10B408) || x > sg(0x10B40C)) { e.H(0x98DC0, msg); return 0; }
    const int32_t top = Y0 + Hh - 0x6D;
    if (y >= top && y <= sg(0x10B414)) {
        const int32_t n = sg(0xE11C8);
        int32_t sel = -1, found = 0, cur = 0;
        for (int32_t i = 0; i < n; ++i) {
            if (G(e, 0x10B23C + i * 0x14) == 1) { cur = i; found = 1; }
            const int32_t l = sg(0xE11D0 + (G(e, 0xE11C4) << 4) + i * 4) + sg(0x11B9A0);
            if (sg(0x10B2A4) > l && sg(0x10B2A4) < l + 0x2D && sg(0x10B430) > top && sg(0x10B430) < Y0 + sg(0xDBC4C) - 0x41) sel = i;
        }
        if (sel != -1) {
            const uint32_t sf = 0x10B23C + (uint32_t)sel * 0x14;
            if (G(e, 0x10B238 + (uint32_t)sel * 0x14) != 1) { e.H(0x8D300); S(e, 0x11BEEC, 0); }
            else {
                c88a70(1);
                if (found != 1) { S(e, sf, 1); S(e, 0x11BEEC, 1); }
                else if (sel != cur) { S(e, sf, 1); S(e, 0x10B23C + (uint32_t)cur * 0x14, 0); S(e, 0x11BEEC, 1); }
                else {
                    S(e, sf, 0);
                    S(e, 0x11BC34, 1);
                    const uint32_t pl = G(e, 0x11BBFC);
                    if (pl && e.R(pl + 0x60) && e.H(0xC202, e.R(pl + 0x60)) == 0) {
                        S(e, 0x11B834, 0); S(e, 0x11B864, 1); S(e, 0x11B854, 5); S(e, 0x11B858, 0);
                    }
                    S(e, 0x11BEEC, 1);
                }
            }
        }
        o511();
        return 0;
    }
    if (y < sg(0x10B410) || y > sg(0x10B414)) { e.H(0x98DC0, msg); return 0; }
    if (e.H(0x8BDE0) && x >= sg(0x10B3B4) && x <= sg(0x10B3B8) && y >= sg(0x10B3BC) && y <= sg(0x10B3C0) && G(e, 0x119721)) {
        const uint32_t p = lookup();
        if (p) {
            if (e.R(p) != 1) { const uint32_t eax = fatal5a(); itemCall(eax, (uint32_t)y); }
            else itemCall(p, (uint32_t)y);
        }
    }
    o511();
    return 0;
}
