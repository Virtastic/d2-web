#pragma once
#include <cstdint>
#include <cstring>

enum { GF_STATE, GF_F174, GF_F1B4, GF_F0A, GF_NEXT, GF_NAMEA, GF_NAMEC, GF_NCLW };   // client +4, +0x174 (unit index or 0), +0x1B4, +0xA (u16), +0x4A8, account name seed (+0xD), char name seed (+0x1D)
enum { GF_LOG = 64, GF_STREAM = 16, GF_NS = 5, GF_IN = 8 + 4 * GF_NCLW + 4 + GF_NS * GF_STREAM, GF_OUT = 4 + 3 + 8 * GF_LOG };
struct GfRec {
    uint32_t frame, ptrNull, flagByte, g28, cb, nCl, pad0, pad1;   // game +0xA8; +0x1D28 is null; byte at (+0x1D28)+8; game +0x28 (u16); callback table (0 none, 1 slot +0x14 empty, 2 fn); chain length
    uint32_t cl[4][GF_NCLW], un[4], s[GF_NS][GF_STREAM];            // un: unit +4; s: scripted results, 0 2D670 out flag, 1 B306, 2 4AD10 (unit index), 3 A42A, 4 A2DA
    uint32_t st[4], exited, nCalls, over, log[GF_LOG][8];           // outputs: client states after the run, exit flag, seam-call log
};
static_assert(sizeof(GfRec) == 4 * (GF_IN + GF_OUT), "GfRec layout (shared with D2VecGameFrame)");

enum { GF_GAME = 0xA1, GF_OUTP = 0x99 };
static inline uint32_t gfHC(uint32_t i) { return 0xC0 + i; }       // symbolic handles: client i 0xC0+i, its account name 0xE0+i, unit k 0xD0+k
static inline uint32_t gfHN(uint32_t i) { return 0xE0 + i; }
static inline uint32_t gfHU(uint32_t k) { return k ? 0xD0 + k : 0; }

// the fixture name of a seed: length 0..max, letters from the seed
static inline uint32_t gfName(uint32_t seed, uint32_t max, char* out)
{
    const uint32_t n = seed % (max + 1);
    for (uint32_t i = 0; i < n; i++) out[i] = 'A' + ((seed >> 8) + i * 7) % 26;
    out[n] = 0; return n;
}
static inline void gfCopy(char* d, const char* s, uint32_t max) { uint32_t i = 0; for (; i + 1 < max && s[i]; i++) d[i] = s[i]; d[i] = 0; }   // SStrCopy

struct GfCtx {
    GfRec* r; uint32_t n[GF_NS];
    uint32_t stream(uint32_t k) { return n[k] < GF_STREAM ? r->s[k][n[k]++] : 0; }
    uint32_t call(uint32_t id, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0, uint32_t e = 0, uint32_t f = 0, uint32_t g = 0) {
        if (r->nCalls >= GF_LOG) { ++r->over; r->nCalls++; }
        else { uint32_t* q = r->log[r->nCalls++]; q[0] = id; q[1] = a; q[2] = b; q[3] = c; q[4] = d; q[5] = e; q[6] = f; q[7] = g; }
        return id == 4 ? stream(0) : id == 7 ? stream(1) : id == 9 ? stream(2) : id == 13 ? stream(3) : id == 1 ? stream(4) : 0;
    }
};

// 1.13c D2Game+0x2D9B0: the per-frame client state machine (stdcall, game; ret 4). Written from the 1.13c decompile ONE definition, included by the capture
// (the retail capture tool/game-frame.inc, i686) as the model retail must agree with, and by the wasm replay (game_gameframe.cpp). Clients are 1-based indices into rec.cl (0 ends a chain),
// units 1-based indices into rec.un; every callee is a logged seam call whose result is scripted by the record.
static inline void GF_2D9B0(GfCtx& x)
{
    GfRec& r = *x.r;
    auto fatal = [&](uint32_t code) { const uint32_t m = x.call(1, code); x.call(2, 0x6FD19FF9u, m); x.call(3, 0xFFFFFFFFu); r.exited = 1; };
    uint32_t v = r.frame & 0x80001FFFu;
    if ((int32_t)v < 0) v = ((v - 1) | 0xFFFFE000u) + 1;
    const bool gate = v == 0;
    if (r.ptrNull) { fatal(0x27); return; }
    if (!(r.flagByte & 4)) { fatal(0xCDF); return; }
    const uint32_t out = x.call(4, GF_GAME, GF_OUTP);
    if (gate && out == 0) x.call(5, GF_GAME);
    auto pkt4 = [&](uint32_t me, uint32_t* c) {
        x.call(8, gfHC(me), 4, 1); c[GF_STATE] = 4;
        const uint32_t u = x.call(9, 0);
        if (u) x.call(10, GF_GAME, gfHU(u), 1);
    };
    for (uint32_t i = r.nCl ? 1 : 0; i;) {
        uint32_t* c = r.cl[i - 1]; const uint32_t nx = c[GF_NEXT]; const uint32_t me = i; i = nx;
        const uint32_t st = c[GF_STATE];
        if (st == 4) { x.call(6, gfHC(me), GF_GAME); continue; }
        if (st == 5) { x.call(6, gfHC(me), GF_GAME); if (x.call(7, c[GF_F1B4])) pkt4(me, c); continue; }
        if (st != 3) continue;
        x.call(6, gfHC(me), GF_GAME);
        if (!x.call(7, c[GF_F1B4])) continue;
        if (!x.call(9, 0)) continue;
        pkt4(me, c);
        x.call(11, GF_GAME, gfHC(me));
        if (!c[GF_F174]) { fatal(0x14F); return; }
        x.call(12, gfHU(c[GF_F174]), GF_GAME);
        if (r.cb) {
            const uint32_t u = x.call(9, 0);
            if (u && r.cb == 2) {
                const uint32_t a = x.call(13, gfHU(u), 12, 0, c[GF_F0A] & 0xFFFF);
                x.call(14, r.g28 & 0xFFFF, gfHN(me), r.un[u - 1], a);
            }
        }
        char an[32], cn[64]; gfName(c[GF_NAMEA], 15, an); gfName(c[GF_NAMEC], 30, cn);
        uint8_t P[0x2C]; memset(P, 0, sizeof P); P[0] = 0x5A; P[1] = 2; P[2] = 4;
        const uint32_t len = (uint32_t)strlen(cn); *(uint32_t*)(P + 0x28) = len;
        if (len) { gfCopy((char*)P + 0x18, cn, 0x10); P[0x27] = 0; }
        gfCopy((char*)P + 8, an, 0x10);
        uint32_t w[11]; memcpy(w, P, sizeof P);
        x.call(15, w[0], w[1], w[2], w[3], w[4], w[5], w[6]);
        x.call(16, w[7], w[8], w[9], w[10], GF_GAME);
    }
}

static inline void GF_RUN(GfCtx& x)
{
    GfRec& r = *x.r; memset(r.st, 0, sizeof(uint32_t) * GF_OUT); memset(x.n, 0, sizeof x.n);
    GF_2D9B0(x);
    for (int i = 0; i < 4; i++) r.st[i] = r.cl[i][GF_STATE];
}
