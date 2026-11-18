// 1.13c D2Client+0x914F0, written from the disassembly (body 0x914F0..0x923C8, jump table 0x923C8): the item name line.
// stdcall(item, out, n), ret 0xC. Composes the item's display name into the caller's wide buffer `out` (capacity n units) by quality
// (D2Common #10106, nine-way switch), through Unicode strcpy / strcat / strlen / win2Unicode / toUnicode (IAT 0xCEAB8 / 0xCEABC /
// 0xCEAB0 / 0xCEAC0 / 0xCEAC4, run in place), Unicode::Personalize (IAT 0xCEAA4, D2Lang 0x8740, run in place) and the colour prefixer
// 0xBF5F0. Strings come from D2Lang #10003 (thunk 0xD1EC -> slot 0xCEACC, id in CX). Every other callee goes through e.H().
// The locals live in one frame laid out as retail's (esp-relative after the four saved registers). D2Lang's Unicode default ctor
// (#10017, IAT 0xCEB00) stores a 0 unit, so every constructed buffer starts empty: ctor() zeroes it where retail constructs it.
// e.gl = the pinned byte 0x11C390 (the required character level the red-name check compares against).
#include <cstdint>
#include <cstring>
#include "cl_env.h"
extern "C" void d2lang_8740(uint16_t* dst, const uint16_t* p2, const uint16_t* p3, int cap, int lang);
namespace {
using W = char16_t;
constexpr uint32_t kInv = 0xB20D476Cu, kEmpty = 0xB20D359Fu, kPctD = 0xB20D057Cu;   // "inv.cpp", "", "%d" (module strings)
constexpr uint32_t S10003 = 0xCEACC;
uint32_t A(const void* p) { return (uint32_t)(uintptr_t)p; }
W* P(uint32_t a) { return (W*)(uintptr_t)a; }
void cpy(W* d, const W* s) { while ((*d++ = *s++)) {} }
void cat(W* d, const W* s) { while (*d) ++d; cpy(d, s); }
uint32_t len(const W* s) { uint32_t n = 0; while (s[n]) ++n; return n; }
// Unicode::win2Unicode(dst, src, n): bytes zero-extended until NUL or n units; NUL after a short source, else the last unit becomes 0
void win2(W* d, const uint8_t* s, int32_t n) {
    int32_t i = 0;
    for (; i < n && s[i]; ++i) d[i] = (W)s[i];
    if (i < n) d[i] = 0; else if (n > 0) d[n - 1] = 0;
}
// Unicode::toUnicode(dst, src, n): at most n-1 units, NUL-terminated (the ASCII subset of its UTF-8 decode)
void toU(W* d, const uint8_t* s, int32_t n) {
    int32_t i = 0;
    for (; i < n - 1 && s[i]; ++i) d[i] = (W)s[i];
    d[i] = 0;
}
}  // namespace
// 1.13c D2Client+0x914F0
extern "C" void d2c_914f0(const cl::Env& e, uint32_t item, uint32_t outA, uint32_t n) {
    if (!item) return;
    alignas(4) uint8_t F[0x1be0];
    memset(F, 0, sizeof F);
    const auto B = [&](uint32_t off) { return (W*)(F + off); };
    const auto D = [&](uint32_t off) -> uint32_t& { return *(uint32_t*)(F + off); };
    const auto ctor = [&](uint32_t off, uint32_t units) { memset(F + off, 0, units * 2); };
    const auto text = [&](uint32_t id) { return P(e.H(S10003, 0, 0, 0, 0, 0, 0, id)); };
    const auto R16 = [&](uint32_t a) { return e.R(a) & 0xffff; };
    // 0xBF5F0 (EDI = buf, byte colour, ret 4): a non-empty buf becomes text(0xF9A) + ('0' + colour) + buf
    const auto colour = [&](W* buf, uint32_t c) {
        if (!buf[0]) return;
        W tmp[0x400] = {};
        cpy(tmp, text(0xf9a));
        const W ch[2] = { (W)(uint8_t)(c + 0x30), 0 };
        cat(tmp, ch);
        cat(tmp, buf);
        cpy(buf, tmp);
    };
    const auto c720 = [&](const W* s) { return P(e.H(0x8c720, 0, 0, 0, 0, 0, 0, 0, 0, 0, A(s))); };   // ESI = the string
    const auto fatal = [&](uint32_t line) {
        const uint32_t m = e.H(0xbf92, line);
        e.H(0xbf80, kEmpty, m);
        e.H(0x3664, 0xffffffffu);   // _exit; a hooked exit returns and retail runs on at 0x922FA
    };
    // "(" + the record's ANSI name + ")" at frame 0x4C; returns its length
    const auto paren = [&](uint32_t name) {
        uint8_t* c = F + 0x4c;
        c[0] = '('; c[1] = 0;
        uint32_t k = 1;
        for (uint32_t i = 0;; ++i) { const uint8_t ch = (uint8_t)e.R8(name + i); c[k++] = ch; if (!ch) break; }
        --k;
        c[k] = ')'; c[k + 1] = 0;
        return k + 1;
    };
    W* const out = P(outA);
    ctor(0xfdc, 0x200); ctor(0xbdc, 0x200); ctor(0x17dc, 0x200);
    D(0x1c) = A(text(0xf9b));
    D(0x20) = A(text(0xf9e));
    W* const sep = P(D(0x20));
    D(0x14) = 0;
    const uint32_t rec = e.H(0xc1ba, e.R(item + 4));
    D(0x10) = rec;
    if (!rec) { fatal(0x732); return; }   // retail runs on and faults on [0 + 0xF4]
    const uint32_t c11088 = e.H(0xc220, item);
    W* tailp = nullptr;   // non-null: the 0x9233C tail (strcpy(out, 0x8C720(tailp)))
    bool f922fa = false;
    if (e.H(0xc1a8, item, 0x4000000, 0x735, kInv)) {
        ctor(0x3dc, 0x200);
        cpy(B(0x3dc), text(R16(rec + 0xf4)));
        cpy(B(0xfdc), c720(B(0x3dc)));
        cat(B(0xfdc), sep);
        const uint32_t id = e.H(0xc8da, item, 0);
        W* t = text(id);
        if (!t) t = P(D(0x1c));
        cpy(B(0xbdc), t);
        cat(B(0x17dc), B(0xbdc));
        colour(B(0x17dc), 4);
        cat(out, B(0xfdc));
        cat(out, B(0x17dc));
    } else if (!e.H(0xc1a8, item, 0x10, 0x748, kInv)) {
        f922fa = true;
    } else {
        const uint32_t q = e.H(0xc19c, item);
        const auto fmt5 = [&](W* dst, uint32_t a1, uint32_t idA, uint32_t idB) {   // 0x916F0: 0x90CB0(dst, text(idB), text(idA), a1, 0)
            W* t2 = text(idA);
            W* t3 = text(idB);
            e.H(0x90cb0, A(dst), A(t3), A(t2), a1);
        };
        switch (q) {
        case 1: {
            const uint32_t r = e.H(0xc8f2, e.H(0xc808, item));
            if (!r) break;
            W* t1 = text(R16(rec + 0xf4));
            fmt5(out, A(t1), R16(r + 0x20), 0x6b0);
            break;
        }
        case 2: {
            if (e.H(0xc1cc, item, 0x16) || e.H(0xc1cc, item, 0x12)) {
                const uint32_t v = e.H(0xc886, item, 0) & 0xffff;
                uint32_t id;
                if (v == 0) id = c11088 == 0x12 ? 0x897 : 0x898;
                else if (v == 1) id = c11088 == 0x12 ? 0x899 : 0x89a;
                else break;
                cpy(out, text(id));
                break;
            }
            if (e.H(0xc1cc, item, 7)) {
                memset(F + 0x24, 0, 0x12);
                ctor(0x4c, 0x100);
                ctor(0x14, 3);
                const uint32_t v = e.H(0xc982, item) & 0xff;
                e.H(0xbea2, A(F + 0x24), 3, kPctD, v);
                win2(B(0x14), F + 0x24, 3);
                ctor(0x3dc, 0x100);
                const uint32_t nm = e.H(0xc7ea, item);
                for (uint32_t i = 0;; ++i) { const uint8_t ch = (uint8_t)e.R8(nm + i); F[0x38 + i] = ch; if (!ch) break; }
                win2(B(0x4c), F + 0x38, 0x100);
                if (e.H(0xc1a8, item, 0x8000, 0x775, kInv)) {
                    cpy(out, text(0x1406));
                    cat(out, sep);
                    cat(out, text(0x102d));
                } else {
                    cpy(out, text(0x102d));
                }
                cat(out, P(D(0x1c)));
                cat(out, B(0x14));
                cat(out, sep);
                const uint32_t c = e.H(0xc808, item);
                cat(out, P(e.H(0x8c210, 0, 0, 0, 0, 0, c)));
                cat(out, sep);
                B(0x24c)[0] = 0; ctor(0x24e, 0x63);
                ctor(0x13dc, 0x200);
                cpy(B(0x13dc), text(R16(rec + 0xf4)));
                const uint32_t lang = e.H(0xd1e6);
                d2lang_8740((uint16_t*)B(0x24c), (const uint16_t*)B(0x4c), (const uint16_t*)B(0x13dc), 0x64, (int)lang);
                cat(out, B(0x24c));
                break;
            }
            if (e.H(0xc1cc, item, 0x28)) {
                const uint32_t r = e.H(0x1230, 0, 0, 0, 0, 0, e.H(0xc808, item));
                W* t1 = text(R16(rec + 0xf4));
                fmt5(out, A(t1), R16(r + 6), 0x6b4);
                break;
            }
            const uint32_t u60 = e.R(item + 0x60);
            if (e.H(0xc1a8, item, 0x800, 0x799, kInv) && u60 && e.H(0xc8d4, u60)) {
                W* t1 = text(R16(rec + 0xf4));
                fmt5(out, A(t1), 0x6c0, 0x6b3);
                break;
            }
            ctor(0x13dc, 0x200);
            cpy(B(0x13dc), text(R16(rec + 0xf4)));
            tailp = B(0x13dc);
            break;
        }
        case 3: {
            W* t1 = text(R16(rec + 0xf4));
            fmt5(out, A(t1), 0x6bf, 0x6af);
            break;
        }
        case 6: case 8: case 9: {
            const uint32_t r1 = e.H(0xc892, e.H(0xc946, item) & 0xffff);
            const uint32_t r2 = e.H(0xc892, e.H(0xc8e6, item) & 0xffff);
            D(0x1c) = r2;
            if (r1) {
                const uint32_t id = R16(r1 + 0xc);
                if (id) { D(0x14) = len(text(id)); cpy(B(0xfdc), text(id)); }
                else { D(0x14) = paren(r1 + 0x26); toU(B(0xfdc), F + 0x4c, 0x200); }
            } else {
                B(0xfdc)[0] = 0;
            }
            if (r2) {
                const uint32_t id = R16(r2 + 0xc);
                W* s;
                if (id) { D(0x14) += len(text(id)); s = text(id); }
                else { ctor(0x3dc, 0x200); paren(r2 + 0x26); toU(B(0x3dc), F + 0x4c, 0x200); D(0x14) += len(B(0x3dc)); s = B(0x3dc); }
                if (r1) cat(B(0xbdc), s); else cpy(B(0xbdc), s);
            } else {
                B(0xbdc)[0] = 0;
            }
            if ((int32_t)(D(0x14) + len(text(R16(rec + 0xf4))) + 3) >= (int32_t)n) { fatal(0x822); f922fa = true; break; }
            ctor(0x13dc, 0x200);
            cpy(B(0x13dc), text(R16(rec + 0xf4)));
            cpy(B(0x17dc), c720(B(0x13dc)));
            cat(B(0x17dc), sep);
            B(0x24c)[0] = 0; ctor(0x24e, 0xc7);
            W* t = text(0x6b6);
            e.H(0x90cb0, A(B(0x24c)), A(t), A(B(0xfdc)), A(B(0xbdc)));
            ctor(0x7dc, 0x200);
            const uint32_t ok = e.H(0x8dfb0, A(B(0x7dc)), A(B(0x24c)), 0, 0, 0, 0, 0, 0, item);
            cat(B(0x17dc), ok ? B(0x7dc) : B(0x24c));
            cat(out, B(0x17dc));
            break;
        }
        case 7: {
            ctor(0x3dc, 0x200);
            cpy(B(0x3dc), text(R16(rec + 0xf4)));
            if ((int32_t)e.H(0xc808, item) < 0) { tailp = B(0x3dc); break; }
            const uint32_t r = e.H(0x18a0, 0, 0, 0, 0, 0, e.H(0xc808, item));
            D(0x1c) = r;
            if (!r) { tailp = B(0x3dc); break; }
            if (!e.R8(rec + 0x144)) { cpy(out, c720(B(0x3dc))); cat(out, sep); }
            ctor(0x7dc, 0x200);
            W* p = c720(text(R16(r + 0x22)));
            if (e.H(0x8dfb0, A(B(0x7dc)), A(p), 0, 0, 0, 0, 0, 0, item)) cat(out, B(0x7dc));
            else cat(out, c720(text(R16(r + 0x22))));
            break;
        }
        case 5: {
            const uint32_t r = e.H(0x1870, 0, 0, 0, 0, 0, e.H(0xc808, item));
            if (!r) break;
            ctor(0x7dc, 0x200);
            cpy(B(0x7dc), text(R16(rec + 0xf4)));
            cpy(out, c720(B(0x7dc)));
            cat(out, sep);
            if (!R16(r + 0x24)) break;
            ctor(0x13dc, 0x200); ctor(0x3dc, 0x200);
            cpy(B(0x3dc), text(R16(r + 0x24)));
            if (e.H(0x8dfb0, A(B(0x13dc)), A(B(0x3dc)), 0, 0, 0, 0, 0, 0, item)) { cat(out, B(0x13dc)); break; }
            B(0x24c)[0] = 0; ctor(0x24e, 0x63);
            W* t = text(0x2769);
            e.H(0x90cb0, A(B(0x24c)), A(t), A(B(0x3dc)), 0);
            cat(out, B(0x24c));
            break;
        }
        case 4: {
            const uint32_t pre = e.H(0xc8b0, e.H(0xc8da, item, 0) & 0xffff);
            const uint32_t suf = e.H(0xc8b0, e.H(0xc886, item, 0) & 0xffff);
            uint32_t ebx;
            if (pre) {
                const uint32_t id = R16(pre + 0x20);
                if (id) { D(0x14) = len(text(id)); cpy(B(0xfdc), text(id)); ebx = D(0x14); }
                else { ebx = paren(pre); toU(B(0xfdc), F + 0x4c, 0x200); }
            } else {
                B(0xfdc)[0] = 0; ebx = D(0x14);
            }
            if (suf) {
                const uint32_t id = R16(suf + 0x20);
                if (id) { ebx += len(text(id)); cat(B(0xbdc), text(id)); }
                else { ctor(0x7dc, 0x200); ebx += paren(suf); toU(B(0x7dc), F + 0x4c, 0x200); cat(B(0xbdc), B(0x7dc)); }
            } else {
                B(0xbdc)[0] = 0;
            }
            if ((int32_t)(ebx + len(text(R16(rec + 0xf4))) + 3) >= (int32_t)n) { fatal(0x8c5); f922fa = true; break; }
            W* t1 = text(R16(rec + 0xf4));
            W* t2 = text(0x6b2);
            e.H(0x90cb0, outA, A(t2), A(B(0xfdc)), A(t1));   // retail pushes six words: + the suffix buffer 0xBDC and 0 (words 4 / 5, not logged)
            break;
        }
        default:
            fatal(0x8ce);
            f922fa = true;
        }
    }
    if (f922fa) {
        ctor(0x7dc, 0x200);
        cpy(B(0x7dc), text(R16(rec + 0xf4)));
        tailp = B(0x7dc);
    }
    if (tailp) cpy(out, c720(tailp));
    e.H(0x8e080, outA, 0, 0, 0, 0, 0, 0, 0, item);
    if (!e.R8(rec + 0x12a)) return;
    if (e.R8(rec + 0x12b)) {
        const uint32_t bl = e.R8(e.gl);
        if ((int32_t)e.H(0xc1c6, item, 0x164, 0) < (int32_t)bl) { colour(out, 1); return; }
    }
    if (e.R(rec + 0x80) != 0x2067656cu) colour(out, 4);
}
