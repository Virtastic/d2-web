// 1.13c D2Game+0x2CA20 / 0x29BE0 / 0x2CF30 / 0x2C0F0 (kinds 3473-3476): the four GAME_UpdateProgress callees that only call other functions. Written from the 1.13c decompile etc.
// ONE definition, included by the capture (the retail capture tool/game-misc.inc, i686) as the model retail must agree with, and by the wasm replay (game_gamemisc.cpp).
// Every callee is a logged seam call; its result is scripted from the record's input words (the result a callee returns is part of the model, not of retail).
#pragma once
#include <cstdint>
enum { GM_IN = 64, GM_EX = 8, GM_LOG = 64, GM_OUT = 2 + GM_EX + GM_LOG * 5 };
struct GmRec { uint32_t in[GM_IN], nCalls, over, ex[GM_EX], log[GM_LOG][5]; };
enum : uint32_t { GM_GAME = 0xA1, GM_ACT = 0xB1, GM_CL = 0xC1, GM_UN = 0xD1, GM_RM = 0xE0, GM_ND = 0x100 };   // symbolic handles: game, act a, client k, unit k, room 4a+j, node
static inline uint32_t gmNode(uint32_t a, uint32_t j, uint32_t k) { return GM_ND + (a * 4 + j) * 4 + k; }

struct GmCtx {
    GmRec* r; uint32_t fn, o[3], cc;   // cc: client being processed (2CA20)
    uint32_t call(uint32_t id, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0) {
        if (r->nCalls >= GM_LOG) { ++r->over; r->nCalls++; } else { uint32_t* q = r->log[r->nCalls++]; q[0] = id; q[1] = a; q[2] = b; q[3] = c; q[4] = d; }
        const uint32_t* in = r->in;
        if (fn == 0x29BE0) return id == 1 ? in[0] : in[1];
        if (fn == 0x2CF30) {   // 1 first room of act (a), 2 tile count of room, 3 flag(act, room), 4/5 nothing
            if (id == 1) { uint32_t a_ = a - GM_ACT; return in[5 + a_] ? GM_RM + a_ * 4 : 0; }
            if (id == 2 || id == 3) { uint32_t rm = (id == 2 ? a : b) - GM_RM, w = in[10 + (rm >> 2) * 3 + (rm & 3)]; return id == 2 ? (uint32_t)(int32_t)(int16_t)(w >> 16) : (w >> 8) & 1; }
            return 0;
        }
        if (fn == 0x2CA20) {   // 1 ordinal 11045(act, i), 2 ordinal 11062 (three out values), 3 SUnit lookup -> unit handle, 4..6 nothing
            if (id == 1) return in[5 + b];
            if (id == 2) { uint32_t a_ = a - GM_ACT; o[0] = in[10 + 3 * a_]; o[1] = in[11 + 3 * a_]; o[2] = in[12 + 3 * a_]; return 0; }
            if (id == 3) return in[33 + 8 * cc];
            return 0;
        }
        return 0;
    }
};

static inline void GM_2C0F0(GmCtx& x)   // in[0] = game +0xA8 frame
{
    if ((int32_t)x.r->in[0] % 0x5DC == 0) for (uint32_t i = 0; i < 5; i++) { x.call(1, GM_GAME, i); x.call(2, GM_GAME, i); }
}

// in: [0] tick, [1] ordinal 10147 result, [2] +0xB0, [3] +0xB4, [4] +0x1C, [5] image global 0x10E360, [6] 0x111C34, [7] 0x111F88, [8] +0xAC
// ex: +0xAC, +0xB0, +0xB4, 0x10E360, 0x111C34, 0x111F88
static inline void GM_29BE0(GmCtx& x)
{
    const uint32_t* in = x.r->in; uint32_t* e = x.r->ex;
    uint32_t ac = in[8], b0 = in[2], b4 = in[3], g1 = in[5], g2 = in[6], g3 = in[7];
    const uint32_t t = x.call(1), el = t - b4;
    if (el > 999) {
        g1 = ac = (b0 * 1000u) / el; b0 = 0; b4 = t;
        const uint32_t u = x.call(2, in[4]);
        if (t - g3 > 10000) { g2 = u; g3 = t; }
        else if (g2 < u) g2 = u;
    }
    b0++; e[0] = ac; e[1] = b0; e[2] = b4; e[3] = g1; e[4] = g2; e[5] = g3;
}

// in: [0..4] act present, [5..9] rooms per act (0-2), [10+3a+j] room word = units | flag10163 << 8 | tile count << 16
static inline void GM_2CF30(GmCtx& x)
{
    const uint32_t* in = x.r->in;
    for (uint32_t a = 0; a < 5; a++) {
        if (!in[a]) continue;
        const uint32_t act = GM_ACT + a;
        uint32_t j = 0;
        for (uint32_t room = x.call(1, act); room; room = ++j < in[5 + a] ? GM_RM + a * 4 + j : 0) {
            if ((int32_t)x.call(2, room) > 10 && x.call(3, act, room)) {
                for (uint32_t k = 0, n = in[10 + a * 3 + j] & 0xFF; k < n; k++) x.call(4, gmNode(a, j, k), GM_GAME);
                x.call(5, act, room);
            }
        }
    }
}

// in: [0..4] act present, [5..9] ordinal 11045 result, [10+3a..12+3a] ordinal 11062 outs, [25] clients (0-3), [26+8c..33+8c] client c: flags, +0x174 (0-3), +0x170, +0x16C, +0x1A8, +4, +0x1AC, SUnit result (0-3)
// ex: the three clients' +0x174 after
static inline void GM_2CA20(GmCtx& x)
{
    const uint32_t* in = x.r->in; uint32_t cur[3] = { in[27], in[35], in[43] };
    for (uint32_t i = 0; i < 5; i++) {
        if (!in[i] || !x.call(1, GM_ACT + i, i)) continue;
        x.call(2, GM_ACT + i);
        const uint32_t v1 = x.o[0], v2 = x.o[1], v3 = x.o[2];
        const uint32_t w0 = 0x53u | v1 << 8, w1 = v1 >> 24 | v2 << 8, w2 = v2 >> 24 | (v3 & 0xFF) << 8;
        for (uint32_t c = 0; c < in[25] && c < 3; c++) {
            const uint32_t* w = in + 26 + 8 * c; uint32_t unit = 0;
            if (w[0] & 1) unit = cur[c];
            else if (cur[c]) { x.cc = c; unit = x.call(3, w[4], w[2], w[3]); if (!unit) cur[c] = 0; }
            if (!unit) continue;
            const uint32_t uh = GM_UN + unit;
            x.call(4, GM_GAME, uh, 0); x.call(5, uh, 1);
            if (w[5] == 4 && (w[6] & 0xFF) == i) x.call(6, GM_CL + c, w0, w1, w2);
        }
    }
    for (uint32_t c = 0; c < 3; c++) x.r->ex[c] = cur[c];
}

static inline void GM_RUN(GmCtx& x)
{
    GmRec& r = *x.r; r.nCalls = r.over = 0;
    for (auto& e : r.ex) e = 0;
    for (auto& q : r.log) for (auto& w : q) w = 0;
    switch (x.fn) { case 0x2C0F0: GM_2C0F0(x); break; case 0x29BE0: GM_29BE0(x); break; case 0x2CF30: GM_2CF30(x); break; case 0x2CA20: GM_2CA20(x); break; }
}
