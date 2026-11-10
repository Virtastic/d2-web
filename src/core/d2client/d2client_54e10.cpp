// 1.13c D2Client+0x54E10, written from the disassembly: the item-property line builder.
// Convention: EAX = property index, ESI = the Unicode output line, 4 stack words (unit, a1, a2, a3), ret 0x10; a3 only feeds 0x546E0, the kinds read a2; returns 1 when a line
// was produced (or the property's kind took the tail path), 0 otherwise.
// Locals (retail esp offsets after the three saves): +0x0c label, +0x10 separator (0xF9B), +0x1c 0xFA1, +0x20 plus sign (0xFA2), +0x24 mode,
// +0x28 kind (first the 0x54AC0 out-flag), +0x30 wide number (10 elements), +0x48 narrow number (10 bytes), +0x54 tail id, +0x5c the
// 0x100-element scratch line.
// Callees (H(rva, s0..s4, eax, ecx, edx, ebx, esi)): 0x546E0 (unit, idx; EAX = a3, EDI = record), 0x54AC0 (6 words, the last an out-flag),
// the 10003 thunk 0xD1EC (CX = string id), Storm 578 through thunk 0xBEA2 (cdecl buf, 10, "%i", value), the D2Lang Unicode imports
// (ctor 0xCEB00 ECX, strcpy 0xCEAB8 / strcat 0xCEABC ECX dst EDX src, toUnicode 0xCEAC4 ECX dst EDX src + 1 word, sprintf 0xCEAB4 cdecl
// size, dst, fmt, varargs), the D2Common thunks 0xC5FE (11044), 0xC8C2 (10449), 0xCCB2 (10000), and the local getters 0x1230, 0x1300,
// 0x1E10, 0x15B0 (EAX) and 0x2FD10 (EAX + 1 word).
#include <cstring>
#include "cl_env.h"
namespace {
constexpr uint32_t kFmtI = 0xB20D34C4u;   // "%i" in D2Client .rdata (RVA 0xD34C4), logged as a module pointer
constexpr uint16_t kAuraTbl[4] = { 0x52f3, 0x52f5, 0x52f2, 0x52f4 };   // the word table at RVA 0xD5E20, clamped index 0..3
}
// 1.13c D2Client+0x54E10
extern "C" uint32_t d2c_54e10(uint32_t idx, uint32_t out, uint32_t unit, uint32_t a1, uint32_t a2, uint32_t a3, const cl::Env& e) {
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    const auto W16 = [&](uint32_t a) { e.W(a, e.R(a) & 0xffff0000u); };
    W16(out);
    if ((int32_t)idx < 0 || (int32_t)idx >= (int32_t)e.R(e.dt + 0xbd4)) return 0;
    const uint32_t rec = idx * 0x144u + e.R(e.dt + 0xbcc);
    if (!rec) return 0;
    char16_t w30[10], w5c[0x100]; char c48[10]; uint32_t flag = 1, a = 0, n = 0, t2c = 0, t58 = 0, io = 0;
    memset(w30, 0, sizeof w30); memset(w5c, 0, sizeof w5c); memset(c48, 0, sizeof c48);
    const auto D = [&](uint32_t id) { return e.H(0xd1ec, 0, 0, 0, 0, 0, 0, id & 0xffffu); };
    const auto cpy = [&](uint32_t d, uint32_t s) { e.H(0xceab8, 0, 0, 0, 0, 0, 0, d, s); };
    const auto cat = [&](uint32_t d, uint32_t s) { e.H(0xceabc, 0, 0, 0, 0, 0, 0, d, s); };
    const auto ctor = [&](char16_t* p, uint32_t k) { for (uint32_t i = 0; i < k; ++i) e.H(0xceb00, 0, 0, 0, 0, 0, 0, P(p + i)); };
    const auto num = [&](int32_t v) { e.H(0xbea2, P(c48), 10, kFmtI, (uint32_t)v); e.H(0xceac4, 9, 0, 0, 0, 0, 0, P(w30), P(c48)); };
    const auto spf = [&](uint32_t sz, uint32_t dst, uint32_t fmt, uint32_t v0, uint32_t nx = 0, uint32_t x0 = 0, uint32_t x1 = 0, uint32_t x2 = 0) {
        e.H(0xceab4, sz, dst, fmt, v0); const uint32_t xw[3] = { x0, x1, x2 }; e.Xw(0xceab4, xw, nx);
    };
    int32_t v = (int32_t)e.H(0x546e0, unit, idx, 0, 0, 0, a3); e.Edj(0x546e0, rec);
    a3 = a2;   // 546E0 took the fourth stack word; every later read of [esp+0x268] is three pushes deeper, i.e. the third word
    e.Xs(P(&flag));
    const uint32_t two = e.H(0x54ac0, unit, a1, idx, (uint32_t)v, rec, rec, P(&flag));
    if (two && !flag) return 0;
    uint32_t kind = e.R8(rec + 0x36), mode = e.R8(rec + 0x37), tail = e.R(rec + 0x3c) & 0xffffu, lid;
    if (two) { kind = e.R8(rec + 0x40); mode = e.R8(rec + 0x41); tail = e.R(rec + 0x46) & 0xffffu; lid = e.R(rec + (v >= 0 ? 0x42 : 0x44)) & 0xffffu; }
    else lid = e.R(rec + (v >= 0 ? 0x38 : 0x3a)) & 0xffffu;
    const uint32_t label = D(lid);
    e.H(0xbea2, P(c48), 10, kFmtI, (uint32_t)v);   // entry: 578, then the ten w30 constructors, then the widening
    ctor(w30, 10);
    e.H(0xceac4, 9, 0, 0, 0, 0, 0, P(w30), P(c48));
    const uint32_t s240 = D(0xfa1), sep = D(0xf9b), plus = D(0xfa2);
    const uint32_t W30 = P(w30), W5C = P(w5c);
    // mode 1: <value text> <sep> <label>; mode 2: <label> <sep> <value text>; other modes: label or nothing (per kind)
    const auto m12 = [&](uint32_t text, bool cpyText) {   // the shared tail of kinds 1/6/12, 3/9, 13, 25/26 (text first or label first)
        if (mode == 1) { cpy(out, text); cat(out, sep); cat(out, label); }
        else if (mode == 2) { cpy(out, label); cat(out, sep); cat(out, text); }
        (void)cpyText;
    };
    bool ok = true;   // false: retail's return-0 exit (no tail)
    switch (kind) {
    case 1: case 6: case 12:
        ctor(w5c, 0x100); w5c[0] = 0;
        if (v > 0) { if (!(kind == 12 && v <= 1)) { cpy(W5C, plus); cat(W5C, W30); } }
        else cpy(W5C, W30);
        m12(W5C, true);
        break;
    case 20: case 21: v = (int32_t)(0u - (uint32_t)v); num(v); [[fallthrough]];
    case 4: case 8:
        if (mode == 1) { if (v >= 0) { cpy(out, plus); cat(out, W30); } else { W16(out); cat(out, W30); } cat(out, s240); cat(out, sep); cat(out, label); }
        else if (mode == 2) { cpy(out, label); cat(out, sep); if (v >= 0) cat(out, plus); cat(out, W30); cat(out, s240); }
        else cpy(out, label);
        break;
    case 3: case 9:
        if (mode == 1 || mode == 2) m12(W30, true); else cpy(out, label);
        break;
    case 5: case 10: v = (int32_t)((uint32_t)v * 100u) / 128; num(v); [[fallthrough]];
    case 2: case 7:
        if (mode == 1) { cpy(out, W30); cat(out, s240); cat(out, sep); cat(out, label); }
        else if (mode == 2) { cpy(out, label); cat(out, sep); cat(out, W30); cat(out, s240); }
        else cpy(out, label);
        break;
    case 11:
        if (v <= 0) spf(0x80, out, D(0x52f9), 0x19);
        else { const int32_t q = 2500 / v; if (q <= 30) spf(0x80, out, D(0x52f9), 1); else { const uint32_t f = D(0x52fa); spf(0x80, out, f, 1, 1, (uint32_t)((q + 12) / 25)); } }
        break;
    case 13: {
        ctor(w5c, 0x100); w5c[0] = 0;
        if (v > 0) { cpy(W5C, plus); cat(W5C, W30); } else if (v == 0) { ok = false; break; } else cpy(W5C, W30);
        const uint32_t r = e.H(0x1300, 0, 0, 0, 0, 0, a3);
        if (!r) { ok = false; break; }
        const uint32_t l2 = D(e.R(r + 0x52) & 0xffffu);
        if (mode == 1) { cpy(out, W5C); cat(out, sep); cat(out, l2); } else if (mode == 2) { cpy(out, l2); cat(out, sep); cat(out, W5C); }
        break; }
    case 14: {
        const uint32_t r = e.H(0x1300, 0, 0, 0, 0, 0, (uint32_t)((int32_t)a3 >> 3)), k = a3 & 7;
        if (!r || k >= 3) { ok = false; break; }
        spf(0x100, out, D(e.R(r + 0x54 + k * 2) & 0xffffu), (uint32_t)v);
        cat(out, sep); cat(out, D(e.R(r + 0x5a) & 0xffffu));
        break; }
    case 15: {
        const int32_t sh = (int32_t)a3 >> (e.R(e.dt + 0xc6c) & 31); const uint32_t m = e.R(e.dt + 0xc70) & a3;
        if (sh <= 0 || sh >= (int32_t)e.R(e.dt + 0xba0)) { ok = false; break; }
        const uint32_t s = D(e.H(0x2fd10, 0, 0, 0, 0, 0, (uint32_t)sh));
        spf(0x100, out, D(e.R(rec + 0x38) & 0xffffu), (uint32_t)v, 3, 0, m, s);
        break; }
    case 16: {
        const uint32_t x = D(e.H(0x2fd10, 0, 0, 0, 0, 0, a3));
        if (!x) { ok = false; break; }
        spf(0x100, out, label, (uint32_t)v, 1, x);
        break; }
    case 17: case 18: {
        ctor(w5c, 0x100); w5c[0] = 0;
        a = 0; n = 0;
        const uint32_t g = e.R(e.gl);
        if (g) a = e.H(0xc5fe, g, P(&n));
        e.Xs(P(&t58));
        int32_t r = (int32_t)e.H(0xc8c2, (uint32_t)v, a, n, P(&a), P(&t2c), P(&t2c), P(&t58));
        if (!e.R(e.gl)) r = (int32_t)t2c;
        int32_t c = (int32_t)a; c = c <= 0 ? 0 : c >= 3 ? 3 : c;
        a = (uint32_t)c;
        cat(out, D(kAuraTbl[c])); cat(out, D(0xf9e));
        e.H(0xbea2, P(c48), 10, kFmtI, (uint32_t)r); e.H(0xceac4, 9, 0, 0, 0, 0, 0, W30, P(c48));
        if (r >= 0) { cpy(W5C, plus); cat(W5C, W30); } else if (v < 0) cpy(W5C, W30);
        if (kind == 18) cat(W5C, s240);
        if (mode == 1) { cat(out, W5C); cat(out, sep); cat(out, label); } else if (mode == 2) { cat(out, label); cat(out, sep); cat(out, W5C); }
        break; }
    case 19: spf(0x100, out, label, (uint32_t)v); break;
    case 22: {
        if (mode == 1) { if (v >= 0) cpy(out, plus); else W16(out); cat(out, W30); cat(out, s240); cat(out, sep); cat(out, label); }
        else if (mode == 2) { cpy(out, label); cat(out, sep); if (v >= 0) cat(out, plus); cat(out, W30); cat(out, s240); }
        else cpy(out, label);
        const uint32_t r = e.H(0x1e10, 0, 0, 0, 0, 0, a3);
        if (r) { cat(out, D(0xf9d)); cat(out, D(0xf9b)); cat(out, D(e.R(r + 0xa) & 0xffffu)); }
        break; }
    case 23: {
        const uint32_t r = e.H(0x1230, 0, 0, 0, 0, 0, a3);
        if (!r) { ok = false; break; }
        if (mode == 1) { cpy(out, W30); cat(out, s240); cat(out, sep); cat(out, label); }
        else if (mode == 2) { cpy(out, label); cat(out, sep); cat(out, W30); cat(out, s240); }
        else cpy(out, label);
        cat(out, D(0xf9b)); cat(out, D(e.R(r + 6) & 0xffffu));
        break; }
    case 24: {
        const uint32_t m = e.R(e.dt + 0xc70) & a3, hi = (uint32_t)(v >> 8), lo = (uint32_t)v & 0xff;
        const uint32_t x = D(e.H(0x2fd10, 0, 0, 0, 0, 0, (uint32_t)((int32_t)a3 >> (e.R(e.dt + 0xc6c) & 31))));
        if (!x) break;
        ctor(w5c, 0x100); w5c[0] = 0;
        cat(out, D(0x5301)); cat(out, sep);
        e.H(0xbea2, P(c48), 10, kFmtI, m); e.H(0xceac4, 9, 0, 0, 0, 0, 0, W30, P(c48));
        cat(out, W30); cat(out, sep); cat(out, x); cat(out, sep);
        spf(0x80, W5C, label, lo, 1, hi);
        cat(out, W5C);
        break; }
    case 25: case 26:
        ctor(w5c, 0x100); w5c[0] = 0;
        if ((int32_t)(0u - (uint32_t)v) > 0) { cpy(W5C, plus); cat(W5C, W30); } else cpy(W5C, W30);
        m12(W5C, true);
        break;
    case 27: {
        const uint32_t x = D(0xfa3);
        if (!x || !v) break;
        const uint32_t y = D(e.H(0x2fd10, 0, 0, 0, 0, 0, a3));
        if (!y) break;
        if (v > 0) { cat(out, plus); cat(out, W30); } else cpy(out, W30);
        cat(out, sep); cat(out, x); cat(out, sep); cat(out, y); cat(out, sep);
        io = 7;
        if (!e.H(0xccb2, a3, P(&io)) || (int32_t)io < 0 || (int32_t)io >= 7) break;
        const uint32_t r = e.H(0x1300, 0, 0, 0, 0, 0, io);
        if (!r) break;
        cat(out, D(e.R(r + 0x5a) & 0xffffu));
        break; }
    case 28: {
        if (!v) break;
        const uint32_t q = e.H(0x15b0, 0, 0, 0, 0, 0, a3);
        if (!q) break;
        uint32_t u = unit; if (!u || e.R(u)) u = e.R(e.self);
        const int32_t pid = u ? (int32_t)e.R(u + 4) : -1;
        if ((int32_t)(int8_t)e.R8(q + 0xc) == pid && v > 3) v = 3;
        const uint32_t x = D(0xfa3);
        if (!x) break;
        const uint32_t y = D(e.H(0x2fd10, 0, 0, 0, 0, 0, a3));
        if (!y) break;
        num(v);
        if (v > 0) { cpy(out, plus); cat(out, W30); } else cpy(out, W30);
        cat(out, sep); cat(out, x); cat(out, sep); cat(out, y);
        break; }
    default: return 0;
    }
    if (!ok) return 0;
    if ((kind >= 6 && kind <= 10) || kind == 21) {
        if (tail != 0x1506) { cat(out, sep); cat(out, D(tail)); }
        else { const uint32_t t = D(0x2b53); cat(out, sep); cat(out, t); }
    }
    return 1;
}
