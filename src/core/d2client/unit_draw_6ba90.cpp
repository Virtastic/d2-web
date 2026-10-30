// D2Client+0x6BA90: per-unit draw (unit in EAX). Env-seam port: every call site of the 1.13c routine is one
// Env::call(id, words, outs), numbered as in the FUNCTION-mode capture (record kind 3481):
//  1 GetStat(unit,0x92)  2 0x6AA30  3 #10036  4 #10013  5 0x1A80  6 #10083(0)  7 #10013  8 #11080  9 #11168  10 #10012
//  11 #10023  12 #10651  13 #11142  14 #10688  15 0x6A3D0  16 #10175  17 #10350  18 #10569  19 GetState(unit,0x25)
//  20 0xA60D0  21 #10431  22 0x69910  23 0x6A2F0  24 0x69090  25 #10663  26 0x6DCA0  27 0x6D6D0  28 0x6EA50
//  29 0x6D740  30 0x6E2C0  31 #10011  32 fatal(line)  33/34/35 #10083(#10036 result)
// w[] holds the register/stack arguments the capture logs; o[] receives the
// callee's out-parameters (ids 11, 17, 18, 21, 26, 28). Pointers returned by 14, 20, 24, 27 are opaque to the port.
#include <cstdint>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;

struct D2cUnitDrawEnv {
    u32 (*rd32)(u32 addr);
    u32 (*rd8)(u32 addr);
    u32 (*call)(u32 id, const u32* w, u32* o);
};

extern "C" {
// 1.13c D2Client+0x6BA90; returns EAX (0 or 1; 0 when the fatal site is reached).
EMSCRIPTEN_KEEPALIVE u32 d2c_6ba90(const D2cUnitDrawEnv* e, u32 unit, u32 gBBFC, u32 g9960, u32 gC418, u32 g995c,
                                   u32 gC414, u32 g8bc48) {
    u32 w[6], o[3];
    auto C = [&](u32 id, u32 a = 0, u32 b = 0, u32 c = 0, u32 d = 0, u32 f = 0, u32 g = 0) {
        w[0] = a; w[1] = b; w[2] = c; w[3] = d; w[4] = f; w[5] = g; o[0] = o[1] = o[2] = 0;
        return e->call(id, w, o);
    };
    if (unit && ((e->rd32(unit + 0xc4) >> 5) & 1)) return 0;
    if (C(1, unit, 0x92)) return 0;
    i32 type = unit ? (i32)e->rd32(unit) : -1;
    if (!unit || type < 0 || type > 2) { C(2, unit); return 1; }
    u32 hb = 0;                                   // [esp+0x10]: #10083 owed on exit
    u32 v24 = C(3);
    if (v24 && C(4) && unit == gBBFC && !C(5, unit)) { C(6, 0); hb = 1; }
    u32 v0c = C(7);
    u32 info = e->rd32(unit + 0x54);
    u32 ext = info ? e->rd32(info + 0x30) : 0;
    auto X = [&](u32 off) { return ext ? e->rd32(ext + off) : 0u; };
    u32 xc, yc;
    if (v0c) {
        i32 v = (i32)X(0xc); u32 half = (u32)((v + (v < 0 ? 1 : 0)) >> 1);
        u32 x = C(8, unit) - half;
        u32 y = C(9, unit) - half;
        if (!C(10, x, y)) { if (hb) C(33, v24); return 0; }
        C(11, x, y, 0);
        xc = o[0] + (g9960 - gC418);
        yc = o[1] + g995c + 8;
    } else {
        i32 v = (i32)X(0x3c); u32 half = (u32)(v / 2);
        xc = C(12, unit) + half;
        yc = C(13, unit) + half;
    }
    if (hb) yc -= 8;
    xc += X(0x34);
    yc += X(0x38);
    u32 sx = (gC418 - g9960) + xc - 2;
    u32 sy = yc - g995c + 8;
    if (type == 2) {
        u32 t = C(14, e->rd32(unit + 4));
        sx += e->rd32(t + 0x148); sy += e->rd32(t + 0x14c);
    }
    if (v0c) {
        i32 g = (i32)g8bc48; u32 q = (u32)((g + ((g >> 31) & 3)) >> 2);
        if (gC414 == 1) sx -= q; else if (gC414 == 2) sx += q;
    }
    if (!C(15, unit, sx, sy, 1)) { if (hb) C(34, v24); return 0; }
    u32 tp = (u32)type, id = e->rd32(unit + 4), r175 = C(16, unit);
    C(17, unit, tp, id, r175, 0x926); tp = o[0]; id = o[1]; r175 = o[2];   // w1..w3 are the pre-call locals
    C(18, unit); u32 o1 = o[0];
    u32 p60 = e->rd32(unit + 0x60);
    if (((e->rd32(unit + 0xc8) >> 3) & 1) && C(19, unit, 0x25)) { u32 ow = C(20, unit); if (ow) p60 = e->rd32(ow + 0x60); }
    u32 tok = C(21, unit, p60, r175, 0);
    if (!C(22, unit, 0xFFFFFFFFu) && !C(23, 0xFFFFFFFFu, tok, unit)) return 0;
    u32 map = C(24, unit, r175);
    if (!map) return 0;
    u32 cls = C(25, unit) & 0xff;
    C(26, unit, tp, id, r175, cls, e->rd8(map + 2)); cls = o[0]; u32 o2 = o[1];
    u32 buf = C(27, map, o2, o1);
    if (!buf) { C(32, 0x94e); return 0; }
    if ((i32)C(28, unit, map, buf, cls, o1) <= 0) { if (hb) C(34, v24); return 0; }
    for (u32 i = 0; e->rd8(map) && i < e->rd8(map); i++) {
        u32 b = e->rd8(buf + i);
        if (b >= 0x10) { C(32, 0x960); return 0; }
        if (C(29, map, b)) {
            u32 rec1 = b << 8;
            if (C(30, 0xFFFFFFFFu, unit, o1, rec1, cls)) C(31, o1, rec1, cls, sx, sy);
        }
    }
    if (hb) C(35, v24);
    return 1;
}
}

// Replay probe for the retail captures (kind 3481, D2VecClient6BA90, 512 words): rec = one record; out = {result, fatal, n, log[48][7]}.
// The fixture block is rebuilt exactly as the capture does (canonical base 0xF1000000); callees answer from the record's script.
namespace {
struct P6 { const u32* r; uint8_t fx[0x1000]; u32 n, fatal, iterA, iterB; u32* log; };
P6 g6;
u32 rd32_6(u32 a) { u32 o = a - 0xF1000000u; return o < 0xFFD ? *(u32*)(g6.fx + o) : 0; }
u32 rd8_6(u32 a) { u32 o = a - 0xF1000000u; return o < 0x1000 ? g6.fx[o] : 0; }
u32 call6(u32 id, const u32* w, u32* o) {
    if (g6.n < 48) { u32* l = g6.log + g6.n * 7; l[0] = id; for (int i = 0; i < 6; i++) l[1 + i] = w[i]; }
    g6.n++;
    const u32* r = g6.r; u32 ret = r[29 + id];
    const u32* os = r + 65 + id * 3;
    switch (id) {
    case 11: o[0] = os[0]; o[1] = os[1]; break;
    case 17: case 26: o[0] = os[0]; o[1] = os[1]; o[2] = os[2]; break;
    case 18: o[0] = os[0]; o[1] = os[1]; break;
    case 21: case 28: o[0] = os[0]; break;
    case 14: ret = 0xF1000280u; break;
    case 20: ret = ret ? 0xF1000180u : 0; break;
    case 24: ret = ret ? 0xF1000400u : 0; break;
    case 27: ret = ret ? 0xF1000480u : 0; break;
    case 29: ret = g6.iterA < 32 ? (r[22] >> g6.iterA) & 1 : 0; g6.iterA++; break;
    case 30: ret = g6.iterB < 32 ? (r[23] >> g6.iterB) & 1 : 0; g6.iterB++; break;
    case 32: g6.fatal = w[0]; break;
    default: break;
    }
    return ret;
}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_6ba90(const u32* r, u32* out) {
    for (auto& b : g6.fx) b = 0;
    auto W = [&](u32 off, u32 v) { *(u32*)(g6.fx + off) = v; };
    W(0x00, r[1]); W(0x04, r[2]); W(0x60, r[3]); W(0xC4, r[4]); W(0xC8, r[5]);
    if (r[6]) { W(0x54, 0xF1000100u); if (r[6] > 1) { W(0x100 + 0x30, 0xF1000140u); W(0x140 + 0xC, r[7]); W(0x140 + 0x34, r[8]); W(0x140 + 0x38, r[9]); W(0x140 + 0x3C, r[10]); } }
    W(0x180 + 0x60, r[19]); W(0x280 + 0x148, r[20]); W(0x280 + 0x14C, r[21]);
    g6.fx[0x400] = (uint8_t)r[17]; g6.fx[0x402] = (uint8_t)r[18]; for (int i = 0; i < 16; i++) g6.fx[0x480 + i] = ((const uint8_t*)(r + 25))[i];
    g6.r = r; g6.n = g6.fatal = g6.iterA = g6.iterB = 0; g6.log = out + 3;
    for (int i = 0; i < 3 + 48 * 7; i++) out[i] = 0;
    D2cUnitDrawEnv e{ rd32_6, rd8_6, call6 };
    u32 res = d2c_6ba90(&e, r[0] ? 0xF1000000u : 0u, r[11] ? 0xF1000000u : 0xC0DE0001u, r[12], r[13], r[14], r[15], r[16]);
    out[0] = res; out[1] = g6.fatal; out[2] = g6.n;
}
