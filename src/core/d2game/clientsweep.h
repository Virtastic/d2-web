// 1.13c D2Game+0x2C830 / +0x2D670 / +0x2C120: the three client-list walkers under the per-frame client state machine (0x2D9B0).
// Written from the 1.13c decompile. ONE definition, included by the capture (the retail capture tool/main.cpp, i686) as the
// model it must agree with retail on, and by the wasm replay (game_clientsweep.cpp). Clients are 1-based indices into rec.cl (0 ends a chain);
// units are 1-based indices into rec.un; every callee is a logged seam call (SwCtx::call) whose result is scripted by the record.
#pragma once
#include <cstdint>
#include <cstring>

// client words: f0a (u16 at +0xA), flags (+0x3D4), f174 (save unit, unit index or 0), f170, f16c, f1a8 (the three E03A0 key words), t3d8 (+0x3D8 last-heard tick), f500, f504, next (+0x4A8)
enum { SW_F0A, SW_FLAGS, SW_F174, SW_F170, SW_F16C, SW_F1A8, SW_T3D8, SW_F500, SW_F504, SW_NEXT, SW_NCLW };
enum { SW_UF4, SW_UF0C, SW_UF60, SW_NUW };   // unit +4, +0xC, +0x60
enum { SW_LOG = 48, SW_OUT = 8 + 4 * SW_NCLW + 4 * SW_NUW + 32 };
struct SwRec {
    uint32_t nCl, target, gate, tick, cb, h74, outIn, pad;   // chain length; 2C120 client (1-based, 0 = null); 2D670 gate global; GetTickCount; callback-table state (0 none, 1 no fn, 2 fn); game +0x74; 2D670 *out before
    uint32_t cl[4][SW_NCLW], un[4][SW_NUW], s[4][8];         // s: scripted results, stream 0 E03A0, 1 A3E2 / list iteration, 2 A4AE, 3 A2DA
    uint32_t cf174o[4], outFlag, exited, nCalls, over, log[SW_LOG][8];
};
static_assert(sizeof(SwRec) == 4 * (SW_OUT + 8 + 8 * SW_LOG), "SwRec layout (shared with D2VecSweep)");

struct SwCtx {
    SwRec* r; uint32_t fn, n[4];   // fn: 0x2C830 / 0x2D670 / 0x2C120
    uint32_t stream(uint32_t k) { return n[k] < 8 ? r->s[k][n[k]++] : 0; }
    uint32_t script(uint32_t id) {   // result of seam call `id` (ids per function in the spec)
        if (fn == 0x2C830) return id == 1 ? stream(0) : (id == 3 || id == 4) ? stream(1) : 0;
        if (fn == 0x2D670) return id == 1 ? r->tick : id == 2 ? stream(0) : 0;
        return id == 1 ? stream(3) : id == 4 ? stream(0) : (id == 6 || id == 9) ? stream(1) : id == 7 ? stream(2) : 0;
    }
    void log(uint32_t id, const uint32_t* w) {
        if (r->nCalls >= SW_LOG) { ++r->over; return; }
        uint32_t* q = r->log[r->nCalls++]; q[0] = id; for (int k = 0; k < 7; k++) q[1 + k] = w[k];
    }
    uint32_t call(uint32_t id, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0, uint32_t e = 0) {
        const uint32_t w[7] = { a, b, c, d, e, 0, 0 }; log(id, w); return script(id);
    }
};
static inline uint32_t swHC(uint32_t i) { return 0xC0 + i; }   // symbolic handles in the log: game 0xA1, client i 0xC0+i, its name (+13) 0xE0+i, +400 0xF0+i, unit k 0xD0+k
static inline uint32_t swHN(uint32_t i) { return 0xE0 + i; }
static inline uint32_t swHP(uint32_t i) { return 0xF0 + i; }
static inline uint32_t swHU(uint32_t k) { return k ? 0xD0 + k : 0; }
enum { SW_GAME = 0xA1, SW_OUTP = 0x99 };

// The save-unit of client c: flag bit 0 -> +0x174 as is; else +0x174 == 0 -> none; else E03A0(+0x16C, +0x170, +0x1A8) and a failed lookup clears +0x174.
static inline uint32_t swUnit(SwCtx& x, uint32_t* c, uint32_t lookupId)
{
    if (c[SW_FLAGS] & 1) return c[SW_F174];
    if (!c[SW_F174]) return 0;
    const uint32_t u = x.call(lookupId, c[SW_F16C], c[SW_F170], c[SW_F1A8]);
    if (!u) c[SW_F174] = 0;
    return u;
}

// 0x2C830 (stack = game; ret 4): tell the registered callback about every client that has a save unit. Seam: 1 E03A0, 2 54320(name, 0), 3/4 Ordinal_10587 x2, 5 callback.
static inline void SW_2C830(SwCtx& x)
{
    SwRec& r = *x.r;
    for (uint32_t i = r.nCl ? 1 : 0; i;) {
        uint32_t* c = r.cl[i - 1]; const uint32_t nx = c[SW_NEXT]; const uint32_t me = i; i = nx;
        const uint32_t u = swUnit(x, c, 1);
        if (!u) continue;
        x.call(2, swHN(me), 0);
        if (r.cb != 2 || !r.h74) continue;
        const uint32_t a = x.call(3, swHU(u), 13, 0, c[SW_F0A] & 0xFFFF, swHP(me));
        const uint32_t b = x.call(4, swHU(u), 12, 0, a, 0);
        x.call(5, swHN(me), r.un[u - 1][SW_UF4], b);
    }
}

// 0x2D670 (stack = game, int* out; ret 8): heartbeat. Seam: 1 GetTickCount, 2 E03A0, 3 54320(name, 0, ebx = game), 4 2D090(esi = client, game, 0), 5 2C830(game).
static inline void SW_2D670(SwCtx& x)
{
    SwRec& r = *x.r;
    r.outFlag = r.outIn;
    if (!r.gate) return;
    r.outFlag = 0;
    const uint32_t tick = x.call(1);
    for (uint32_t i = r.nCl ? 1 : 0; i;) {
        uint32_t* c = r.cl[i - 1]; const uint32_t nx = c[SW_NEXT]; const uint32_t me = i; i = nx;
        if (tick <= c[SW_T3D8]) continue;
        const uint32_t d = tick - c[SW_T3D8];
        const bool trig = (((c[SW_F0A] & 4) && d > 10000 && c[SW_F500] > 10) || d > 45000 || c[SW_F504] != 0);
        if (!trig || !r.cb) continue;
        swUnit(x, c, 2);
        x.call(3, swHN(me), 0, SW_GAME);
        r.outFlag = 1;
        x.call(4, swHC(me), SW_GAME, 0);
    }
    if (r.outFlag) x.call(5, SW_GAME);
}

// 0x2C120 (stack = game, client; ret 8): a client leaves; tell every other client with a save unit about it. Seam: 1 A2DA, 2 A2C2, 3 exit, 4 E03A0, 5 8E170(ebx, esi, stack),
// 6 A466 (first list item), 7 A4AE (item -> id or -1), 8 AA3E0(client, packet words, len), 9 A484 (next item), 10 B2D40(game, client).
static inline void SW_2C120(SwCtx& x)
{
    SwRec& r = *x.r;
    auto fatal = [&](uint32_t code) { const uint32_t m = x.call(1, code); x.call(2, 0x6FD19FF9u, m); x.call(3, 0xFFFFFFFFu); r.exited = 1; };
    if (!r.target) { fatal(251); return; }
    const uint32_t me = r.target; uint32_t* mc = r.cl[me - 1];
    const uint32_t u = swUnit(x, mc, 4);
    if (!u) { fatal(1386); return; }
    for (uint32_t i = r.nCl ? 1 : 0; i;) {
        uint32_t* c = r.cl[i - 1]; const uint32_t nx = c[SW_NEXT]; const uint32_t cur = i; i = nx;
        if (cur == me) continue;
        const uint32_t o = swUnit(x, c, 4);
        if (!o) continue;
        x.call(5, swHC(cur), swHU(o), swHC(me));
        x.call(5, swHC(me), swHU(u), swHC(cur));
        const uint32_t* ou = r.un[o - 1];
        if (!ou[SW_UF60]) continue;
        for (uint32_t it = x.call(6, ou[SW_UF60]); it; it = x.call(9, it)) {
            const uint32_t id = x.call(7, it);
            if (id == 0xFFFFFFFFu) continue;
            uint8_t pk[10] = { 0x8E, 1 }; memcpy(pk + 2, &ou[SW_UF0C], 4); memcpy(pk + 6, &id, 4);
            uint32_t w0, w1, w2 = 0; memcpy(&w0, pk, 4); memcpy(&w1, pk + 4, 4); memcpy(&w2, pk + 8, 2);
            x.call(8, swHC(me), w0, w1, w2, 10);
        }
    }
    x.call(5, swHC(me), swHU(u), swHC(me));
    x.call(10, SW_GAME, swHC(me));
}

static inline void SW_RUN(SwCtx& x)
{
    SwRec& r = *x.r; memset(&r.cf174o, 0, sizeof(uint32_t) * (SW_OUT + 8 + 8 * SW_LOG - SW_OUT)); memset(x.n, 0, sizeof x.n);
    if (x.fn == 0x2C830) SW_2C830(x); else if (x.fn == 0x2D670) SW_2D670(x); else SW_2C120(x);
    for (int i = 0; i < 4; i++) r.cf174o[i] = r.cl[i][SW_F174];
}
