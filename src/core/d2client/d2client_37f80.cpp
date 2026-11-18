// 1.13c D2Client+0x37F80: edit-box key handler. thiscall(ECX = box), one stack word = the key message (ret 4); returns 1
// (handled) or 0 (a space, escape or CR that no class accepted). Written from the disassembly; every callee is a call-log record through cl::Env::H.
#include "d2client_editbox.h"
extern "C" int32_t d2_client_393c(const char* s);   // atol (0x39C4 jumps to 0x393C)
namespace {
// box fields (packed): +4 key callback, +0x18 wrap width, +0x1c max pixel width, +0x20 numeric flag, +0x24 pass-through flag, +0x28 text (wchar*),
// +0x30 numeric max, +0x34 u16 capacity, +0x36 cursor, +0x3a length, +0x42 enter/escape callback, +0x4e font
inline uint32_t w16(const cl::Env& e, uint32_t a) { return e.R(a) & 0xffff; }
inline void put16(const cl::Env& e, uint32_t a, uint32_t v) { e.W(a, (e.R(a) & 0xffff0000u) | (v & 0xffff)); }
}
extern "C" uint32_t d2c_37f80(uint32_t self, uint32_t msg, const cl::Env& e, const D2cEditSeam& s) {
    auto tail = [&](uint32_t edi) { e.W(msg + 0x18, 1); e.W(msg + 0x1c, edi); e.H(0xbe84, msg); return 1u; };
    if (e.R(self + 0x24) == 0 && (e.R(msg + 0xc) & 0x40000000u)) return 1;
    if (e.R(msg + 8) > 0x7f) { e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0); e.H(0xbe84, msg); return 1; }
    const uint32_t bl = e.R(msg + 8) & 0xff;
    const uint32_t cb = e.R(self + 4);
    uint32_t eax;
    if (cb) eax = e.H(cb & 0xffffff, 0, 0, 0, 0, 0, 0, self, msg);
    else if (e.R(self + 0x20)) eax = (uint32_t)s.isdigit(s.ctx, (int32_t)(int8_t)bl);
    else {
        eax = e.H(0xd3f0, 0, 0, 0, 0, 0, 0, bl);
        if (bl == 0x0d || bl == 0x1b) { e.H(e.R(self + 0x42) & 0xffffff); return tail(0); }
    }
    uint32_t edi = 0;
    const uint32_t buf = e.R(self + 0x28);
    if (bl == 0x08) {
        int32_t len = (int32_t)e.R(self + 0x3a);
        if (len > 0) {
            int32_t cur = (int32_t)e.R(self + 0x36);
            if (cur != len && cur != 0) {
                int32_t i = cur - 1;
                for (;;) {
                    int32_t lim = (int32_t)w16(e, self + 0x34); int32_t l2 = (int32_t)e.R(self + 0x3a);
                    if (l2 < lim) lim = l2;
                    if (i >= lim - 1) break;
                    put16(e, buf + i * 2, w16(e, buf + i * 2 + 2));
                    ++i;
                }
            }
            put16(e, buf + e.R(self + 0x3a) * 2 - 2, 0);
            e.W(self + 0x3a, e.R(self + 0x3a) - 1);
        }
        uint32_t cur = e.R(self + 0x36);
        if (cur != 0) e.W(self + 0x36, cur - 1);
        return tail(edi);
    }
    if (bl == 0x2e) {
        int32_t len = (int32_t)e.R(self + 0x3a);
        if (len > 0) {
            int32_t i = (int32_t)e.R(self + 0x36);
            if (i != len) {
                for (;;) {
                    int32_t lim = (int32_t)w16(e, self + 0x34); int32_t l2 = (int32_t)e.R(self + 0x3a);
                    if (l2 < lim) lim = l2;
                    if (i >= lim) break;
                    put16(e, buf + i * 2, w16(e, buf + i * 2 + 2));
                    ++i;
                }
                put16(e, buf + e.R(self + 0x3a) * 2 - 2, 0);
                e.W(self + 0x3a, e.R(self + 0x3a) - 1);
            }
        }
        return tail(edi);
    }
    if (eax == 0) {
        if (bl == 0x20 || bl == 0x1b || bl == 0x0d) return 0;
        return tail(edi);
    }
    {   // insert the character
        const int32_t cap = (int32_t)w16(e, self + 0x34);
        int32_t len = (int32_t)e.R(self + 0x3a);
        if (len < cap - 1) {
            int32_t cur = (int32_t)e.R(self + 0x36);
            const uint32_t ch = (uint32_t)(int32_t)(int8_t)bl & 0xffff;
            if (cur == len) {
                put16(e, buf + cur * 2, ch);
                cur = (int32_t)e.R(self + 0x36) + 1;
                e.W(self + 0x36, (uint32_t)cur);
                put16(e, e.R(self + 0x28) + cur * 2, 0);
                e.W(self + 0x3a, e.R(self + 0x3a) + 1);
            } else {
                int32_t k = len < cap ? len : cap;
                while (k >= cur) {
                    put16(e, e.R(self + 0x28) + k * 2, w16(e, e.R(self + 0x28) + k * 2 - 2));
                    --k;
                    cur = (int32_t)e.R(self + 0x36);
                }
                cur = (int32_t)e.R(self + 0x36);
                uint32_t l3 = e.R(self + 0x3a);
                if ((int32_t)l3 < (int32_t)w16(e, self + 0x34)) { ++l3; e.W(self + 0x3a, l3); }
                put16(e, e.R(self + 0x28) + e.R(self + 0x3a) * 2, 0);
                put16(e, e.R(self + 0x28) + (uint32_t)cur * 2, ch);
            }
        }
    }
    if (e.R(self + 0x3a) != 0) {
        e.H(0xd3a8, 0, 0, 0, 0, 0, 0, e.R(self + 0x4e));
        uint32_t wout = 0;
        uint32_t node = e.H(0xd378, e.R(self + 0x18), 0, 0, 0, 0, 0, e.R(self + 0x28), (uint32_t)(uintptr_t)&wout);
        while (node) {
            uint32_t txt = e.R(node), nx = e.R(node + 4);
            e.H(0xbf86, 0x203, 0, 0, 0, 0, 0, txt, 0x6fb86918u);
            e.H(0xbf86, 0x204, 0, 0, 0, 0, 0, node, 0x6fb86918u);
            node = nx;
        }
        if ((int32_t)wout > (int32_t)e.R(self + 0x1c)) {
            uint32_t len = e.R(self + 0x3a) - 1, cur = e.R(self + 0x36) - 1;
            e.W(self + 0x36, cur); e.W(self + 0x3a, len);
            put16(e, e.R(self + 0x28) + len * 2, 0);
        }
        edi = 0;
    }
    if (e.R(self + 4) != 0) return tail(edi);
    {
        uint32_t n = e.H(0xceab0, 0, 0, 0, 0, 0, 0, e.R(self + 0x28));
        if (n == 0) return tail(edi);
        if (e.R(self + 0x20) == 0) { e.W(self + 0x3a, e.H(0xceab0, 0, 0, 0, 0, 0, 0, e.R(self + 0x28))); return tail(edi); }   // retail calls strlen a second time
    }
    {
        char loc[0x7e8] = {};
        const uint32_t lp = (uint32_t)(uintptr_t)loc, cap = w16(e, self + 0x34);
        e.H(0xceae8, cap, 0, 0, 0, 0, 0, lp, e.R(self + 0x28));
        uint32_t mx = e.R(self + 0x30);
        uint32_t v = (uint32_t)d2_client_393c(loc);
        if (v > mx) {
            s.sprintf_d(s.ctx, loc, mx);
            const uint32_t b = e.R(self + 0x28), cb2 = w16(e, self + 0x34) * 2;
            uint32_t i = 0;
            for (; i + 4 <= cb2; i += 4) e.W(b + i, 0);
            if (i < cb2) put16(e, b + i, 0);   // cb2 is even: at most one trailing word
            e.H(0xceac0, w16(e, self + 0x34), 0, 0, 0, 0, 0, b, lp);
            const uint32_t n2 = e.H(0xceab0, 0, 0, 0, 0, 0, 0, e.R(self + 0x28));
            e.W(self + 0x3a, n2); e.W(self + 0x36, n2);
        }
    }
    return tail(edi);
}
