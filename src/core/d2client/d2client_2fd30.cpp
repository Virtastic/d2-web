// 1.13c D2Client+0x2FD30: skill-description tooltip layout. stdcall(unit A0, skill A1, row A2), ret 0xC.
// Every callee is reached through Env (the host binds them to the retail D2Client functions); the globals arrive in G and the 0xF5210
// callback / layout-row block in G::R. The Env and G layouts are the contract the capture probe fixes.
#include <cstdint>
#include <cstdio>
#include <cstring>
void D2Lang_Unicode_sprintf(int cap, char16_t* out, const char16_t* fmt, ...);
namespace d2client_2fd30 {
using I = int32_t; using U = uint32_t;
struct G { I a0, a4, gB; U gS, gC, gD; const uint8_t* R; };
struct Env {
    void* c;
    I (*cur)(void*, I); I (*q18)(void*, I, I, I); I (*qid)(void*, I); const uint8_t* (*rec)(void*, I); void (*font)(void*, I);
    const char16_t* (*str)(void*, U); I (*width)(void*, const char16_t*); void (*draw)(void*, const char16_t*, I, I, I, I); void (*bc4e0)(void*, const char16_t*, I, I, I, I);
    I (*cb74)(void*, I, I); void (*upper)(void*, char16_t*);
    void (*t1)(void*, U, I, I, I, I, I, I, I); void (*t2)(void*, U, I, I, I, I, I*, I*, I*, I*);
    void (*d300)(void*, I, I, I, I, I); void (*d3c0)(void*, const char16_t*, const char16_t*, I, I, I, I, I);
};
static U ld32(const uint8_t* p) { U v; std::memcpy(&v, p, 4); return v; }
static unsigned ld16(const uint8_t* p) { uint16_t v; std::memcpy(&v, p, 2); return v; }
static void num(char16_t* dst, I v) { char t[16]; std::snprintf(t, sizeof t, "%d", (int)v); for (int i = 0; i < 31 && t[i]; ++i) dst[i] = (uint8_t)t[i]; }

// 1.13c D2Client+0x2FD30
void sub_2FD30(const Env& e, const G& g, I A0, I A1, I A2) {
    void* c = e.c;
    const I s1c = e.cur(c, A1);
    if (!s1c) return;
    const I s24 = e.q18(c, A0, A1, 1);
    const I id = e.qid(c, A1);
    const uint8_t* rec = e.rec(c, id);
    if (!rec) return;
    e.font(c, 6);
    char16_t title[64] = {};
    const uint8_t* rec2 = e.rec(c, id);
    const char16_t* ts = e.str(c, rec2 ? ld16(rec2 + 0xe) : 0x1506u);
    for (int i = 0; i < 63 && ts[i]; ++i) title[i] = ts[i];
    e.upper(c, title);
    const I a0 = g.a0, ya = (I)((U)g.a4 + (U)g.gB);                         // y rows add a4 + gB, x columns add a0
    auto row = [&](I i, I& rx, I& ry, I& rr) {
        const uint8_t* p = g.R + 0xD8 + 12 * (U)i;
        rx = (I)ld32(p); ry = (I)ld32(p + 4); rr = (I)ld32(p + 8);
    };
    I rx, ry, rr;
    row(A2, rx, ry, rr);
    I x = (I)((U)rx + (U)a0);
    const I boxw = (I)((U)rr - (U)rx + 1u);
    const I tw = e.width(c, title);
    if (tw < boxw) x = (I)((U)x + (U)((boxw - tw) >> 1));
    e.draw(c, title, x, (I)((U)ry + (U)ya - 0x1e0u), 0, 0);

    const I cb = e.cb74(c, A0, A1);
    bool bShort = false, bLong = false;
    I mode;
    if (cb != 0) mode = cb;
    else mode = (A0 == (I)g.gS && g.gC < g.gD) ? 8 : 0;
    if ((U)mode <= 8) {
        static const uint8_t kind[9] = { 0, 0, 1, 1, 1, 1, 0, 1, 0 };
        if (kind[mode] == 0) bShort = bLong = true;
    }
    const unsigned f1 = ld16(rec + 0x12), f2 = ld16(rec + 0x14);
    if (f1 >= 0x90 || ld32(g.R + 4 * f1) == 0) bShort = false;
    if (f2 >= 0x48 || ld32(g.R + 0x90 + 4 * f2) == 0) bLong = false;
    I r = A2 + 1;
    if (bShort) {
        const char16_t* s = e.str(c, 0xfdd);
        row(r, rx, ry, rr);
        e.bc4e0(c, s, (I)((U)rx + (U)a0), (I)((U)rr + (U)a0), (I)((U)ry + (U)ya - 0x1e0u), 0);
    }
    ++r;
    if (bShort) {
        row(r, rx, ry, rr);
        e.t1(c, ld32(g.R + 4 * f1), A0, A1, s1c, s24, rx, ry, rr);
    }
    ++r;
    if (!bLong) return;
    I x4 = 0, x10 = 0, x0 = 0, x20 = 0;
    e.t2(c, ld32(g.R + 0x90 + 4 * f2), A0, A1, s1c, s24, &x4, &x10, &x0, &x20);
    if (x4 != 0 || x0 != 0) {
        e.font(c, 6);
        char16_t buf[200] = {};
        const char16_t* fmt = id == 0 ? e.str(c, 0xfe1) : e.str(c, 0xfdf);
        D2Lang_Unicode_sprintf(0xc8, buf, fmt, (uintptr_t)title);
        buf[199] = 0;
        // 1.13c D2Client+0xBC3E0: split at the first line feed, terminating the first line there
        const char16_t* second = nullptr; bool two = false;
        { int n = 0; while (buf[n]) ++n; int k = 0; while (buf[k] && buf[k] != 10) ++k; if (buf[k] == 10) { buf[k] = 0; if (k < n) { second = buf + k + 1; two = true; } } }
        row(r, rx, ry, rr);
        const I l = (I)((U)rx + (U)a0), rt = (I)((U)rr + (U)a0), y = (I)((U)ry + (U)ya);
        if (two) {
            e.bc4e0(c, buf, l, rt, (I)((U)y - 0x1e4u), 0);
            e.bc4e0(c, second, l, rt, (I)((U)y - 0x1dcu), 0);
        } else {
            e.bc4e0(c, title, l, rt, (I)((U)y - 0x1e0u), 0);           // retail passes the title buffer here, not buf
        }
    }
    r += 2;
    row(r, rx, ry, rr);
    if (x0 != 0) {
        char16_t b3[32] = {}, b4[32] = {};
        num(b3, x4); num(b4, x0);
        e.d3c0(c, b3, b4, rx, ry, rr, x10, x20);
    } else if (x4 != 0) {
        e.d300(c, x4, x10, rx, ry, rr);
    }
}
} // namespace d2client_2fd30
