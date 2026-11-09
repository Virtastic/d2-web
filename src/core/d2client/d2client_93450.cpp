// 1.13c D2Client+0x93450, written from the disassembly: the hovered-item tooltip and the inventory hover hints. stdcall(unit, useSelf),
// ret 8. Before the tooltip it draws the hover hints of the inventory / panel buttons and the expansion hint (D2Win #10085). Then it
// composes the hovered item's text (name, type, the requirement / damage / defence / socket lines) into one wide string through Unicode
// strcpy / strcat / strlen (IAT 0xCEAB8 / 0xCEABC / 0xCEAB0, run in place) and the colour prefixer 0xBF5F0, measures it with D2Win
// #10177 (thunk 0xD3B4 -> slot 0xCEBA0) and draws it with D2Win #10085 (thunk 0xD384 -> slot 0xCEBC8). Strings come from D2Lang #10003
// (thunk 0xD1EC -> slot 0xCEACC, id in CX). Set items hand over to 0x928C0 (stdcall, the caller's own first argument, ret 4).
// The locals live in one frame laid out as retail's (esp-relative, 0x6650 bytes plus the four saved registers). Every Unicode buffer is
// built by D2Lang ??_FUnicode (IAT 0xCEB00), which stores 0 in each char, so the zeroed frame here is exact.
// e.gl = the module globals, four ranges back to back: 0x1197A8..0x11C477 at +0, 0xFAD84..0xFAE13 at +0x2CD0, 0xDBC48..0xDBC4F at +0x2D60,
// 0xE0EA4..0xE0EB7 at +0x2D68.
#include <cstdint>
#include <cstring>
#include "cl_env.h"
namespace {
using W = char16_t;
constexpr uint32_t kEmpty = 0xB20D359Fu, kInv = 0xB20D476Cu;   // "" and "inv.cpp" (module strings)
constexpr uint32_t S10003 = 0xCEACC, S10177 = 0xCEBA0, S10085 = 0xCEBC8;
uint32_t G(uint32_t rva) {
    if (rva >= 0x1197a8) return rva - 0x1197a8;
    if (rva >= 0xfad84) return 0x2cd0 + (rva - 0xfad84);
    if (rva >= 0xe0ea4) return 0x2d68 + (rva - 0xe0ea4);
    return 0x2d60 + (rva - 0xdbc48);
}
uint32_t A(const void* p) { return (uint32_t)(uintptr_t)p; }
W* P(uint32_t a) { return (W*)(uintptr_t)a; }
void cpy(W* d, const W* s) { while ((*d++ = *s++)) {} }
void cat(W* d, const W* s) { while (*d) ++d; cpy(d, s); }
uint32_t len(const W* s) { uint32_t n = 0; while (s[n]) ++n; return n; }
struct T {
    const cl::Env& e;
    W* text(uint32_t id) const { return P(e.H(S10003, 0, 0, 0, 0, 0, 0, id)); }
    // 0xBF5F0 (EDI = buf, byte colour, ret 4): a non-empty buf becomes text(0xF9A) + ('0' + colour) + buf; returns buf
    W* colour(W* buf, uint32_t c) const {
        if (!buf[0]) return buf;
        W tmp[0x1100] = {};   // retail: 0x400 chars; larger here so a long label cannot overrun
        cpy(tmp, text(0xf9a));
        const W ch[2] = { (W)(uint8_t)(c + 0x30), 0 };
        cat(tmp, ch);
        cat(tmp, buf);
        cpy(buf, tmp);
        return buf;
    }
    uint32_t hE(uint32_t rva, uint32_t edi, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0) const {
        e.Ed(edi);
        const uint32_t r = e.H(rva, s0, s1, s2);
        e.Ej(rva, edi);
        return r;
    }
    void draw(uint32_t buf, uint32_t x, uint32_t y, uint32_t s1 = 0) const { e.H(S10085, y, s1, 1, 0, 0, 0, buf, x); }
};
// mode - 1 -> hint id (the byte / dword jump tables at 0x6FB4455C / 0x6FB4454C), 0x101D past 14
constexpr uint8_t kModeIx[15] = { 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 2, 2, 3, 2 };
constexpr uint32_t kModeId[4] = { 0x1003, 0x1021, 0x101e, 0x101d };
// item type - 0x3C -> type-name id (0x6FB4458C / 0x6FB4456C), 7 = none
constexpr uint8_t kTypeIx[29] = { 0, 1, 2, 3, 4, 5, 6, 5, 4, 2, 3, 1, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 5 };
constexpr uint32_t kTypeId[7] = { 0x2aa5, 0x2aa9, 0x2aa7, 0x2aa8, 0x2aa6, 0x2aab, 0x2aaa };
// quality - 4 -> colour (0x6FB445AC)
constexpr uint32_t kQualCol[6] = { 3, 2, 9, 4, 8, 10 };
constexpr uint32_t kCodes[11] = { 0x20686563, 0x20746562, 0x20736574, 0x20646566, 0x20616f74, 0x206e6864, 0x20796562, 0x2072626d,
                                  0x20316b70, 0x20326b70, 0x20336b70 };
}  // namespace
// 1.13c D2Client+0x93450
extern "C" void d2c_93450(const cl::Env& e, uint32_t arg1, uint32_t arg2) {
    const T t{ e };
    const auto R = [&](uint32_t rva) { return e.R(e.gl + G(rva)); };
    const auto Ri = [&](uint32_t rva) { return (int32_t)R(rva); };
    static_assert(sizeof(W) == 2, "UCS-2");
    alignas(4) uint8_t F[0x6660];
    memset(F, 0, sizeof F);
    const auto B = [&](uint32_t off) { return (W*)(F + off); };
    const auto D = [&](uint32_t off) -> uint32_t& { return *(uint32_t*)(F + off); };
    // frame: 0x10 who, 0x14 rec, 0x18 sep, 0x1C measured height, 0x20/0x24/0x28/0x2C colour flags, 0x30/0x3C/0x44 the 0xC898 out-flags,
    // 0x34 / 0x48 two zero colours, 0x38 measured width, 0x40 quality, then the UCS-2 buffers
    W* const b60 = B(0x60); W* const ba60 = B(0xa60); W* const b860 = B(0x860); W* const b1460 = B(0x1460); W* const b2060 = B(0x2060);
    W* const b1260 = B(0x1260); W* const b3e60 = B(0x3e60); W* const b3060 = B(0x3060); W* const b4660 = B(0x4660); W* const b3860 = B(0x3860);
    W* const b2260 = B(0x2260); W* const b1e60 = B(0x1e60); W* const b2660 = B(0x2660); W* const b2a60 = B(0x2a60); W* const b2e60 = B(0x2e60);
    W* const b2860 = B(0x2860); W* const b2460 = B(0x2460); W* const b2c60 = B(0x2c60); W* const b1660 = B(0x1660); W* const b1c60 = B(0x1c60);
    W* const b1860 = B(0x1860);
    W* const sep = t.text(0xf9e);
    D(0x18) = A(sep);
    uint32_t who = arg1;
    if (arg2 && R(0x11bbfc)) who = R(0x11bbfc);
    D(0x10) = who;
    if (!R(0x11c474)) {
        const int32_t mx = Ri(0x11b828), xo = Ri(0x11b9a0), yo = Ri(0x11b9a4), sh = Ri(0xdbc4c);
        if (R(0xfae10)) {
            const int32_t y1 = yo + sh - 0x3f;
            if (mx >= xo + 0x110 && mx <= xo + 0x130 && Ri(0x11b824) >= y1 - 0x20 && Ri(0x11b824) <= y1)
                t.draw(A(t.text(0x1030)), (uint32_t)(xo + 0x11f), (uint32_t)(yo + sh - 0x62));
        }
        if (R(0xfad84) || R(0xfaddc) || R(0xfade4) || R(0xfadb0) || R(0xfadb8) || R(0xfade8)) {
            const int32_t my = Ri(0x11b824);
            int32_t ed = Ri(0xdbc48) - xo;
            if (mx >= ed - 0x12e && mx <= ed - 0x10e && my >= yo + sh - 0x60 && my <= yo + sh - 0x40)
                t.draw(A(t.text(0x1030)), (uint32_t)(ed - 0x11f), (uint32_t)(yo + sh - 0x63));
            ed = Ri(0xdbc48) - Ri(0x11b9a0);
            const int32_t yb = yo + sh - 0x45;
            if (mx >= ed - 0xed && mx <= ed - 0xd9 && Ri(0x11b824) >= yb - 0x12 && Ri(0x11b824) <= yb && !e.H(0x45980)) {
                const uint32_t m = R(0x11bc34) - 1;
                const uint32_t id = m > 14 ? 0x101du : kModeId[kModeIx[m]];
                t.draw(A(t.text(id)), (uint32_t)(ed - 0xde), (uint32_t)(yo + sh - 0x59));
            }
            if (e.H(0xbf8c) && R(0x119854) && e.H(0x8c150, 0, 0, 0, 0, 0, R(0x11b824), 0, R(0x11b828))) {
                cpy(b1860, t.text(0x58c6));
                uint32_t d = 1;
                bool add = false;
                if ((e.H(0x38e50, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0x2c) & 0xffff) != 0xffff) { cat(b1860, t.text(0xf9d)); add = true; }
                else if ((e.H(0x38e50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x2c) & 0xffff) != 0xffff) { cat(b1860, t.text(0xf9d)); d = 0; add = true; }
                if (add) cat(b1860, P(e.H(0x39f30, 0, 0, 0, 0, 0, 0x2c, 0, d)));
                const int32_t x0 = Ri(0x11b9a0);
                t.draw(A(b1860), (uint32_t)(x0 + (Ri(0x11b828) > x0 + 0x1f4 ? 0x258 : 0x170)), (uint32_t)(0x15 - Ri(0x11b9a4)));
            }
        }
    }
    if (!R(0x11bc2c) && !R(0x11bc28)) return;
    if (!R(0x11bc38)) return;
    const uint32_t inv = e.R(arg1 + 0x60);
    if (!inv) return;
    if (e.H(0xc202, inv)) return;
    if (!R(0xfaddc) && e.R(arg1 + 0x60) && !e.H(0xc8b6, e.R(arg1 + 0x60), R(0x11bc38))) return;
    const auto drawTail = [&](uint32_t s1) {
        e.H(S10177, A(F + 0x1c), 0, 0, 0, 0, 0, A(ba60), A(F + 0x38));
        const uint32_t c = R(0xe0eac), h = D(0x1c);
        if ((int32_t)(c - h) > 0) t.draw(A(ba60), R(0xe0ea8), c, s1);
        else t.draw(A(ba60), R(0xe0eb0), h + R(0xe0eb4), s1);
    };
    const int32_t mode0 = Ri(0x11bc34);
    if (R(0x11bc28) && !R(0xe0ea4) && R(0x1197a8) && mode0 >= 1 && mode0 <= 9) {
        e.H(0x914f0, R(0x11bc38), A(b3060), 0x80);
        cpy(b860, t.text(0xd7f));
        cat(b860, sep);
        cat(b60, b860);
        const uint32_t ty = e.H(0xc220, R(0x11bc38)) - 0x3c;
        if (ty <= 0x1c && kTypeIx[ty] != 7) {
            cat(b60, t.text(kTypeId[kTypeIx[ty]]));
            cat(b60, sep);
        }
        cat(b60, b3060);
        t.colour(b60, 0);
        e.H(0x91360, A(b60), 0, 0, 0, 0, 0, 0, 0, 0, A(ba60));
        drawTail(0);
        return;
    }
    const uint32_t q = e.H(0xc19c, R(0x11bc38));
    D(0x40) = q;
    if (R(0x11b864) == 8 || e.H(0xc856, R(0x11bc38))) { e.H(0x92440); return; }
    if (q == 5 && e.H(0xc1a8, R(0x11bc38), 0x10, 0xf9f, kInv)) { e.H(0x928c0, arg1); return; }
    if (e.H(0xc220, R(0x11bc38)) == 0x12) { e.H(0x92650); return; }
    const uint32_t kind = e.H(0xc9a0, R(0x11bc38));
    who = D(0x10);
    if (kind != 7) {
        cat(b1460, t.text(kind + 0x2aa5));
        cat(b1460, sep);
        if ((who ? e.R(who + 4) : 0xffffffffu) != kind) D(0x2c) = 1;
    }
    const uint32_t it0 = R(0x11bc38);
    const uint32_t rec = e.H(0xc1ba, it0 ? e.R(it0 + 4) : 0xffffffffu);
    D(0x14) = rec;
    if (!rec) {
        const uint32_t m = e.H(0xbf92, 0xfc2);
        e.H(0xbf80, kEmpty, m);
        e.H(0x3664, 0xffffffffu);
        return;                                           // the fatal exit does not return
    }
    if (e.H(0xc928, R(0x11bc38))) e.H(0x8dee0, R(0x11bc38), rec, 0, 0, 0, 0, 0, 0, 0, A(b60));
    else e.H(0x90770, R(0x11bc38), 0, 0, 0, 0, 0, 0, 0, 0, A(b2660));
    e.Xs(A(F + 0x44));
    e.H(0xc898, R(0x11bc38), who, 0, A(F + 0x3c), A(F + 0x30));
    if (e.H(0xc1a8, R(0x11bc38), 0x10, 0xfd6, kInv)) {
        const int32_t n = (int32_t)e.H(0xc8aa, R(0x11bc38), who);
        if (n > 1) {
            if (!D(0x44)) D(0x28) = 1;
            e.H(0x8de40, (uint32_t)n, 0, 0, 0, 0, 0, 0, 0, 0, A(b2a60));
        }
    }
    if (e.H(0xc1cc, R(0x11bc38), 0x35)) e.H(0x933e0, 0, 0, 0, 0, 0, A(b3860));
    if (e.H(0xc90a, R(0x11bc38))) e.H(0x8d4f0, A(b1e60), 0, 0, 0, 0, R(0x11bc38));
    if (e.H(0xc1cc, R(0x11bc38), 0xd)) e.H(0x8c1e0, 0, 0, 0, 0, 0, 0, 0, 0, 0, A(b2260));
    if (e.H(0xc1cc, R(0x11bc38), 0x2d) || e.H(0xc1cc, R(0x11bc38), 0x32)) {
        e.H(0x8e180, R(0x11bc38), 0, 0, 0, 0, 0, 0, 0, 0, A(b2060));
        uint32_t ebp = 0, edi = 0;
        const uint32_t p = e.H(0xc454, R(0x11bc38), 0x5b, 0);
        if (p) {
            ebp = e.H(0x14a0, 0, 0, 0, 0, 0, p, 100, e.R(rec + 0x10a) & 0xffff);
            edi = e.H(0x14a0, 0, 0, 0, 0, 0, p, 100, e.R(rec + 0x10c) & 0xffff);
        }
        if (e.H(0xc1a8, R(0x11bc38), 0x400000, 0xffa, kInv)) { ebp -= 10; edi -= 10; }
        if (e.R(rec + 0x10c) & 0xffff) {
            if (!D(0x30)) D(0x24) = 1;
            t.hE(0x8dcc0, A(b2860), rec, edi);
        }
        if (e.R(rec + 0x10a) & 0xffff) {
            if (!D(0x3c)) D(0x20) = 1;
            t.hE(0x8dd80, A(b2e60), rec, ebp);
        }
        if (e.H(0xc1cc, R(0x11bc38), 0x2d)) {
            if ((int32_t)e.H(0xc84a, R(0x11bc38), 0) >= 0 && (int32_t)e.H(0xc8ec, R(0x11bc38), 0) >= 0)
                e.H(0x8ff20, R(0x11bc38), A(b2460));
            e.H(0x8f9e0, A(b3e60), 0, 0, 0, 0, R(0x11bc38));
        }
        if (e.H(0xc1cc, R(0x11bc38), 0x33)) {
            const uint32_t w = D(0x10);
            if (w && e.R(w + 4) == 3 && (!e.H(0xc88c, R(0x11bc38)) || e.H(0xc9a0, R(0x11bc38)) == 3))
                t.hE(0x8d940, A(b1660), R(0x11bc38), rec);
            e.H(0x8fdc0, A(b2c60), rec, 0, 0, 0, R(0x11bc38));
        } else if (e.H(0xc1cc, R(0x11bc38), 0xf)) {
            const uint32_t w = D(0x10);
            if (w && e.R(w + 4) == 6) t.hE(0x8d940, A(b1660), R(0x11bc38), rec);
        }
        if (e.H(0xc1cc, R(0x11bc38), 0x32) && (int32_t)e.H(0xc93a, R(0x11bc38)) > 0)
            e.H(0x8fb80, D(0x10), R(0x11bc38), A(b1c60));
    }
    uint32_t col;
    if (D(0x40) - 4 <= 5) col = kQualCol[D(0x40) - 4];
    else if (e.H(0xc1a8, R(0x11bc38), 0x800, 0x1050, kInv)) col = 5;
    else if (e.H(0xc1a8, R(0x11bc38), 0x400000, 0x1054, kInv)) col = 5;
    else col = D(0x48);
    if (!e.H(0xc1a8, R(0x11bc38), 0x10, 0x105d, kInv)) {
        const int32_t g = Ri(0x11bc34);
        if (g >= 1 && g <= 9 && !R(0xe0ea4)) col = 0;
    }
    if (e.H(0xc1a8, R(0x11bc38), 0x10, 0x1069, kInv)) {
        if (!e.H(0xc1a8, R(0x11bc38), 0x800, 0x106c, kInv)) e.H(0x8d880, R(0x11bc38), 0, 0, 0, 0, 0, 0, 0, 0, A(b1260));
        t.hE(0x560b0, A(b4660), R(0x11bc38), 1, 0);
    } else {
        cpy(b860, t.text(0xd7f));
        cat(b860, sep);
    }
    e.H(0x8d600, R(0x11bc38), 0, 0, 0, 0, D(0x14), 0, 0, 0, A(b1260));
    e.H(0x914f0, R(0x11bc38), A(b3060), 0x100);
    cpy(b60, t.colour(b2060, 3));
    const uint32_t code = e.R(D(0x14) + 0x80);
    bool hit = false;
    for (uint32_t c : kCodes) hit |= code == c;
    if (hit || e.H(0xc1cc, R(0x11bc38), 0x4a)) col = 8;
    cat(b60, t.colour(b4660, 3));
    cat(b60, t.colour(b860, 1));
    cat(b60, t.colour(b3e60, 0));
    cat(b60, t.colour(b2a60, D(0x28) & 0xff));
    cat(b60, t.colour(b2e60, D(0x20) & 0xff));
    cat(b60, t.colour(b2860, D(0x24) & 0xff));
    cat(b60, t.colour(b1460, D(0x2c) & 0xff));
    cat(b60, t.colour(b2660, 0));
    cat(b60, t.colour(b3860, 0));
    cat(b60, t.colour(b2260, 0));
    cat(b60, t.colour(b1260, 0));
    cat(b60, t.colour(b2460, 0));
    cat(b60, t.colour(b1660, 0));
    cat(b60, t.colour(b2c60, 0));
    cat(b60, t.colour(b1c60, 0));
    cat(b60, t.colour(b1e60, 4));
    if (e.H(0xc1a8, R(0x11bc38), 0x100, 0x10a7, kInv)) col = 1;
    cat(b60, t.colour(b3060, col & 0xff));
    const int32_t g = Ri(0x11bc34);
    if (g != 0 && g >= 1 && g <= 9) e.H(0x91360, A(b60), 0, 0, 0, 0, 0, 0, 0, 0, A(ba60));
    else cpy(ba60, b60);
    uint32_t s1;
    const uint32_t r2 = D(0x14);
    if ((e.R(r2 + 0x12a) & 0xff) && e.R(r2 + 0x80) != 0x2067656c) {
        const uint32_t c2 = e.R(r2 + 0x80);
        if (!R(0x11bc34)) {
            const uint32_t it = R(0x11bc38);
            if (!it || !e.R(it + 0x10)) {
                const uint32_t id = c2 == 0x20646b62 ? 0x89du : c2 == 0x20786f62 ? 0x89cu : 0u;
                if (id) {
                    cpy(b60, ba60);
                    cpy(ba60, t.text(id));
                    cat(ba60, sep);
                    cat(ba60, b60);
                }
            }
        }
        s1 = 4;
        t.colour(ba60, 4);
    } else {
        s1 = D(0x34);
    }
    if (len(ba60) >= 0x3ff) *(W*)(F + 0x125e) = 0;
    drawTail(s1);
}
