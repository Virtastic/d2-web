// 1.13c D2Client+0x19EF0 / 0x1A010 / 0x1B1D0 / 0x1B360 / 0x1B700: the "action blocked / action done" tail of the UI use-skill path
//. Callees go through D2cHost, in retail order.
#include "client_host.h"
#include <emscripten.h>
static const uint32_t kStk = 0xB3000000u;   // a pointer to a local out-block: its address is not part of the call
static uint32_t cl(D2cHost h, D2cCall c) { return h(&c); }
static void clear150(uint32_t* in) { in[0x54] = in[0x55] = in[0x56] = in[0x57] = 0; }

// 1.13c D2Client+0x1A010: thiscall(ecx), eax live-in, stack (arg1, unit). unit2c = unit[0x2C]. Returns 0 or 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1a010(D2cHost h, uint32_t ecx, uint32_t eax, uint32_t arg1, uint32_t unit, uint32_t unit2c) {
    cl(h, { 0xC3EE, 0, 0, 0, 0, 0, { unit2c, ecx, eax, 0 } });
    if (!cl(h, { 0xC376, 0, 0, 0, 0, 0, { unit2c, unit, 0, 0 } })) return 0;
    cl(h, { 0xCF9A, 0, 0, 0, 0, 0, { unit2c, 0, 0, 0 } });
    cl(h, { 0xCFD0, 0, 0, 0, 0, 0, { unit2c, 0, 0, 0 } });
    cl(h, { 0x2A7C0, unit, 0, 0, 0, 0, { arg1, 0, 0, 0 } });
    return 1;
}

// 1.13c D2Client+0x19EF0: ECX = ctx (words: +4 A, +8 B, +0x18 c6), EAX = edi. A = unit words of ctx[1] (null if 0), inner = A[5] words (null if 0).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_19ef0(D2cHost h, const uint32_t* ctx, uint32_t edi, uint32_t* A, uint32_t* inner) {
    uint32_t a = ctx[1], b = ctx[2];
    if (cl(h, { 0xCC70, 0, 0, 0, 0, 0, { b, a, 0, 0 } }) > 2 || cl(h, { 0xCF0A, 0, 0, 0, 0, 0, { b, a, 0x804, 0 } })) {
        cl(h, { 0x2A7C0, a, 0, 0, 0, 0, { ctx[6], 0, 0, 0 } });
        if (A && A[0] == 0 && inner) { inner[0x54] = 1; inner[0x55] = 0x13; inner[0x56] = 0; inner[0x57] = edi; }
        return;
    }
    if (!cl(h, { 0x29F30, 0, 0, 0, 0, 0, { 0, 0, 0, 0 } })) return;
    cl(h, { 0x2A750, 0, 2, a, 0, 0, { kStk, 0, 0, 0 } });
    if (A && A[0] == 0 && inner) clear150(inner);
}

static int32_t idiv32(uint32_t x, uint32_t y, int32_t d) { int32_t p = (int32_t)(x * y); return d == -1 ? (int32_t)(0u - (uint32_t)p) : p / d; }
// 1.13c D2Client+0x1B1D0: EAX = edi, ESI = ctx (words: +0 flags, +4 A, +8 B, +0x14 c5, +0x1C c7), stack (arg). ctxTok = ctx address token.
// Returns 1; 0xFFFFFFFF = the fatal _exit(-1) paths (A/B null, span <= 0, span-end 0), which are not vectored.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1b1d0(D2cHost h, uint32_t edi, const uint32_t* ctx, uint32_t ctxTok, uint32_t arg) {
    uint32_t a = ctx[1], b = ctx[2];
    if (b == 0 || a == 0) return 0xFFFFFFFFu;
    if (ctx[0] & 0x20) { cl(h, { 0x1A060, 0, 0, 0, 0, 0, { ctxTok, 1, 0, 0 } }); return 1; }
    D2cCall c = { 0x18880, 0, 0, 0, kStk, 0, { ctx[7], b, kStk, 0 } };   // out: *ebx = L0 (the capture cannot write it: 0), *arg3 = L1
    uint32_t r = h(&c);
    int32_t l0 = (int32_t)c.out[0], l1 = (int32_t)c.out[1];
    if (r & 0xFF) { cl(h, { 0x1A060, 0, 0, 0, 0, 0, { ctxTok, 1, 0, 0 } }); return 1; }
    if (ctx[0] & 1) cl(h, { 0x2D50, a, 0, 0, 0, 0, { 6, arg, edi, 0 } });
    if (ctx[0] & 2) cl(h, { 0x2D50, a, 0, 0, 0, 0, { 0xD, arg, edi, 0 } });
    int32_t ebx = l1 - l0;
    if (ebx <= 0 || l1 == 0) return 0xFFFFFFFFu;
    int32_t xa = (int32_t)cl(h, { 0x1630, 0, a, 0, 0, 0, { 0, 0, 0, 0 } });
    int32_t ya = (int32_t)cl(h, { 0x1660, 0, a, 0, 0, 0, { 0, 0, 0, 0 } });
    int32_t xb = (int32_t)cl(h, { 0x1630, 0, b, 0, 0, 0, { 0, 0, 0, 0 } });
    int32_t yb = (int32_t)cl(h, { 0x1660, 0, b, 0, 0, 0, { 0, 0, 0, 0 } });
    int32_t ny = idiv32((uint32_t)(yb - ya), (uint32_t)ebx, l1) + ya;
    int32_t nx = idiv32((uint32_t)(xb - xa), (uint32_t)ebx, l1) + xa;
    cl(h, { 0x1A010, (uint32_t)ny, (uint32_t)nx, 0, 0, 0, { ctx[5], a, 0, 0 } });
    return 1;
}

// 1.13c D2Client+0x1B700: ECX = ctx (as 1B1D0), EAX = edi, stack (ebp). ctxTok / ctx7Tok = addresses of ctx and ctx+0x1C.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1b700(D2cHost h, uint32_t edi, const uint32_t* ctx, uint32_t ctxTok, uint32_t ctx7Tok, uint32_t ebp) {
    uint32_t a = ctx[1], b = ctx[2], c7 = ctx[7];
    if (!cl(h, { 0x18F90, a, 0, 0, ctx7Tok, 0, { 0, 0, 0, 0 } })) return;
    uint32_t kind = cl(h, { 0xD000, 0, 0, 0, 0, 0, { a, c7, 0, 0 } });
    uint32_t p = cl(h, { 0xA4E20, edi & 0x7F, edi, 0xB2000000u | (0x10A608u + (ebp << 9)), 0, 0, { ebp, 0, 0, 0 } });
    if (!p) return;
    uint32_t t = cl(h, { 0xCF04, 0, 0, 0, 0, 0, { p, 0, 0, 0 } });
    if (!cl(h, { 0xC6E2, 0, 0, 0, 0, 0, { a, b, t, 0 } })) {
        if (c7) {
            D2cCall q = { 0xC640, 0, c7, 0, 0, 0, { 0, 0, 0, 0 } };
            if (h(&q) && (uint16_t)q.out[0] == 0x13) { D2cCall z = { 0x1B1D0, edi, 0, 0, 0, ctxTok, { ebp, 0, 0, 0 } }; h(&z); return; }
        }
        if (kind == 1 || kind == 4) {
            if (!(ctx[0] & 0x20)) {
                cl(h, { 0x2A7C0, a, 0, 0, 0, 0, { ctx[6], 0, 0, 0 } });
                if (ctx[0] & 1) cl(h, { 0x2D50, a, 0, 0, 0, 0, { 6, ebp, edi, 0 } });
                if (ctx[0] & 2) cl(h, { 0x2D50, a, 0, 0, 0, 0, { 0xD, ebp, edi, 0 } });
                return;
            }
            cl(h, { 0x1A060, 0, 0, 0, 0, 0, { ctxTok, 0, 0, 0 } });
            return;
        }
    }
    cl(h, { 0x1A060, 0, 0, 0, 0, 0, { ctxTok, 1, 0, 0 } });
}

// 1.13c D2Client+0x1B360: EAX = ctx (words: +4 A, +8 B, +0x14 c5), stack (ebp = record id). tab = the 128-slot record hash table (global 0x10AA08),
// gv = global 0x11BBFC; R resolves a 32-bit field to its struct (replay) . Returns 0xFFFFFFFF on the fatal _exit(-1) paths (not vectored), else 0.
typedef void* (*D2cRes)(uint32_t);
static uint32_t* rs(D2cRes R, uint32_t x) { return x ? (uint32_t*)R(x) : nullptr; }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1b360(D2cHost h, D2cRes R, const uint32_t* ctx, const uint32_t* tab, uint32_t gv, uint32_t ebp) {
    uint32_t* e = rs(R, tab[ebp & 0x7F]);
    while (e && e[3] != ebp) e = rs(R, e[0x39]);
    if (!e) return 0;
    if (e[0] != 2) return 0xFFFFFFFFu;
    if (!cl(h, { 0xC226, 0, 0, 0, 0, 0, { e[1], 0, 0, 0 } })) return 0xFFFFFFFFu;
    bool go = !(e[1] == 0x3B && cl(h, { 0xC1FC, 0, 0, 0, 0, 0, { gv, 0x66, 0, 0 } }));
    uint32_t a = ctx[1], b = ctx[2];
    if (cl(h, { 0xCEFE, 0, 0, 0, 0, 0, { a, b, 0, 0 } }) && !cl(h, { 0xCF0A, 0, 0, 0, 0, 0, { a, b, 0x804, 0 } })) {
        if (!go || !cl(h, { 0x29F30, 0, 0, 0, 0, 0, { 0, 0, 0, 0 } })) return 0;
        cl(h, { 0x2A750, 0, 2, a, 0, 0, { kStk, 0, 0, 0 } });
        uint32_t* A = rs(R, a);
        if (A && A[0] == 0 && A[5]) clear150(rs(R, A[5]));
        return 0;
    }
    uint32_t* B = rs(R, b); int32_t ty = (int32_t)B[0]; uint32_t in = B[0xB];
    bool ph = ty == 2 || (ty > 3 && ty <= 5);
    uint32_t sv, tv;
    if (ph) { uint32_t* I = rs(R, in); sv = I[4]; tv = I[3]; }
    else {
        sv = in ? cl(h, { 0xC2EC, 0, 0, 0, 0, 0, { in, 0, 0, 0 } }) : 0;
        tv = in ? cl(h, { 0xC3BE, 0, 0, 0, 0, 0, { in, 0, 0, 0 } }) : 0;
    }
    cl(h, { 0x1A010, sv, tv, 0, 0, 0, { ctx[5], a, 0, 0 } });
    if (go) cl(h, { 0x2D50, a, 0, 0, 0, 0, { 0x13, 2, ebp, 0 } });
    return 0;
}

// 1.13c D2Client+0x2A7C0: EAX = esi = unit (token), EBX, EDI live-in, stack (param, unused), ret 8.
// A = unit words (null if esi == 0), inner = A[5] words (null if 0). The 7-dword out-block handed to 0x2A750 is a stack local no
// hook can read (ceiling in the spec); its stores are modelled in the order retail makes them.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2a7c0(D2cHost h, uint32_t esi, const uint32_t* A, uint32_t* inner, uint32_t ebx, uint32_t edi, uint32_t param) {
    static const uint32_t kStr = 0xB20D6AD4u;   // module address of the "UnitMode.cpp" path string (RVA 0xD6AD4)
    if (!cl(h, { 0x29F30, 0, 0, 0, 0, esi, { 0, 0, 0, 0 } })) return;
    uint32_t ecx = 0;
    if (param < 1 || param > 0x13 || param == 0xB || param == 0x12) return;   // retail: fatal _exit(-1), not vectored
    if (param == 1 || param == 3 || param == 2 || param == 4) {
        ecx = param == 1 ? 1 : param == 2 ? 0 : param == 3 ? 0x17 : 0x18;
        if ((param == 1 || param == 3) && A && A[0] == 0 && inner && inner[0x55] == 0x13) return;
    } else if (param == 0x13) {
        if (ebx == 4) {
            D2cCall q = { 0xA5B40, 0, edi, ebx, 0, 0, { 0, 0, 0, 0 } };
            if (h(&q) && q.out[0] == ebx && !((q.out[1] >> 1) & 1)) return;
        }
        ecx = 2;
    } else {
        bool pl = param == 5 || param == 6 || param == 7 || param == 8 || param == 9 || param == 10;   // C5BC vs C5CE
        bool a = param == 5 || param == 8 || param == 0xC || param == 0xF;                             // 0x15 vs 0x16
        uint32_t line = pl ? (a ? 0x32C : 0x336) : (a ? 0x33E : 0x348), th = pl ? 0xC5BC : 0xC5CE;
        uint32_t r = cl(h, { th, 0, 0, 0, 0, 0, { esi, 0, 0, 0 } });
        cl(h, { 0xC42A, 0, 0, 0, 0, 0, { r, kStr, line, 0 } });
        r = cl(h, { th, 0, 0, 0, 0, 0, { esi, 0, 0, 0 } });
        cl(h, { 0xC7AE, 0, 0, 0, 0, 0, { r, 0, 0, 0 } });
        ecx = a ? 0x15 : 0x16;
    }
    if (cl(h, { 0x2A750, 0, ecx, esi, 0, 0, { kStk, 0, 0, 0 } })) {
        if (param >= 5 && param <= 0x11) {
            uint32_t g = cl(h, { 0xC250, 0, 0, 0, 0, 0, { esi, 0x148, 0, 0 } });
            cl(h, { 0xC40C, 0, 0, 0, 0, 0, { esi, 0x148, g + 1, 0 } });
        }
        cl(h, { 0x29FA0, param, ebx, edi, 0, 0, { 0, 0, 0, 0 } });
    }
    if (A && A[0] == 0 && inner) clear150(inner);
}

// 1.13c D2Client+0x1B7F0: EAX = ctx (words: +0 flags, +4 A, +8 B, +0xC..+0x14 c3..c5, +0x18 c6, +0x1C c7), no stack args, no result.
// B = ctx[2] words (+0 type, +4, +0xC, +0xC4 bits), null if ctx[2]==0. gm = *0x6FB7EE50 (mask dwords; every test is on the low byte), gm[5] / gm[6] =
// the globals 0x11C2F8 / 0x11C2F4 (written 1 / 0 by the "action started" exits). ctxTok / c7Tok = addresses of ctx and ctx+0x1C.
// 1.13c D2Client+0xA5440 (inlined, not hooked: it touches only EAX/ECX, and 1B7F0 reads the EDX it came in with): B[0], B[4], B[0x31].
static bool a5440(const uint32_t* B) {
    if (B[0x31] & 0x10000) return true;
    return B[0] == 0 ? (B[4] == 0 || B[4] == 0x11) : (B[0] == 1 && (B[4] == 0 || B[4] == 0xC));
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1b7f0(D2cHost h, const uint32_t* c, uint32_t ctxTok, uint32_t c7Tok, const uint32_t* B, uint32_t* gm) {
    const uint32_t flags = c[0], a = c[1], c7 = c[7];
    uint32_t kind, t = 0, t7 = 0, ty, b3, r, out = 0, p7;
    bool eb = false;
    D2cCall q;
    auto go = [&](uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t ebx, uint32_t s0, uint32_t s1, uint32_t s2) { D2cCall k = { rva, eax, ecx, 0, ebx, 0, { s0, s1, s2, 0 } }; return h(&k); };
    auto started = [&] { gm[5] = 1; gm[6] = 0; };
    auto step = [&]() -> bool {   // 0x186A0 then 0x1A010; false = 186A0 failed
        if (!go(0x186A0, 0, 0, 0, a, ctxTok, 0)) return false;
        go(0x1A010, c[4], c[3], 0, c[5], a, 0); started(); return true;
    };
    if (!go(0x19350, ctxTok, 0, 0, 0, 0, 0)) return;
    kind = go(0xD000, 0, 0, 0, a, c7, 0);
    if (c7) { q = { 0xC640, 0, c7, 0, 0, 0, { 0, 0, 0, 0 } }; t = h(&q); if (!t) goto L8b1; t7 = q.out[0] & 0xFF; }
    if (!B) goto L8b1;
    if (!t) goto L9af;
    if ((flags & 2) && kind != 1) {
        if (B[0] == 2 || B[0] == 4) { if (!(gm[3] & t7)) goto L8b1; eb = true; }
        else if (B[0] == 5) goto L8b1;
    }
    if (B[0] != 1 || !a5440(B)) goto L9af;
    if (gm[0] & t7) goto L9af;
L8b1:
    eb = (flags & 0x20) != 0;
    if (!eb && !((flags & 2) && kind != 1)) { step(); return; }
    {   // 0x909
        D2cCall k = { 0x18F90, a, 0, 0, c7Tok, 0, { 0xB3000000u, 0, 0, 0 } };
        if (!h(&k)) return;
        out = k.out[0];
        if (c7 && !go(0xC42A, 0, 0, 0, c7, 0xB2000000u | 0xD6F7Cu, 1967) && out == 1 && !eb) { step(); return; }   // edi=1 and the 98f/994 exit set the same globals
        go(0x1A060, 0, 0, 0, ctxTok, 0, 0); started(); return;
    }
L9af:
    ty = B[0]; b3 = B[3];
    if (!eb && (ty == 2 || (ty > 3 && ty <= 5)) && (flags & 2) && kind != 1) return;   // 0x9cf: kind != 1 returns
    if (!go(0xC7B4, 0, 0, 0, a, c7, 0)) goto L3d;
    if (c7) {
        q = { 0xC640, 0, c7, 0, 0, 0, { 0, 0, 0, 0 } }; p7 = 0;
        if (h(&q)) {
            p7 = q.out[0] & 0xFF;
            if (gm[3] & p7) { if (!go(0x18F90, a, 0, c7Tok, 0, 0, 0)) return; go(0x1A060, 0, 0, 0, ctxTok, 1, 0); return; }
        }
    }
    r = (B[0x31] >> 2) & 1;
    if (!r) goto L10;
    t7 &= 0xFF;
    if (!(gm[1] & t7) && !(gm[2] & t7)) r = go(0xA62A0, c[2], 0, 0, a, 0, 0);
    if (!r) goto L10;
    if (ty == 2) { if (flags & 0x20) return; go(0x1B360, ctxTok, 0, 0, b3, 0, 0); return; }
    {
        uint32_t x = go(0xC208, 0, 0, 0, go(0xC1F0, 0, 0, 0, c[2], 0, 0), 0, 0);
        if (ty == 0) {
            if (x && go(0xC208, 0, 0, 0, go(0xC1F0, 0, 0, 0, a, 0, 0), 0, 0)) { go(0x19EF0, b3, ctxTok, 0, 0, 0, 0); return; }
            go(0x1B700, b3, ctxTok, 0, 0, 0, 0); return;
        }
        if (x && !go(0x18650, c7, 0, 0, 0, 0, 0)) return;
        go(0x1B700, b3, ctxTok, 0, ty, 0, 0); return;
    }
L10:
    if (flags & 0x20) { if (!go(0x18F90, a, 0, c7Tok, 0, 0, 0)) return; go(0x1A060, 0, 0, 0, ctxTok, 0, 0); return; }
    goto L46;
L3d:
    if (flags & 0x20) return;
L46:
    if (ty != 1) { go(0x1AB20, ty, 0, 0, ctxTok, b3, 0); return; }
    if (!(flags & 4)) return;
    {
        D2cCall k = { 0x1230, B[1], 0, 0, 0, 0, { 0, 0, 0, 0 } };
        if (h(&k) && (gm[1] & (k.out[0] & 0xFF))) { go(0x1AB20, 1, 0, 0, ctxTok, b3, 0); return; }
        go(0x2A7C0, a, 0, 1, c[6], 0, 0);
    }
}
