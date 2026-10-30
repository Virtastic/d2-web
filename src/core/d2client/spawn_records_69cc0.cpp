// 1.13c D2Client+0x69CC0 -- spawn-record builder (env-seam port; every callee and memory read goes through the Env).
// Stack ABI (ret 0x10): a1, a2, header (+0 word, +3 byte = sub-record count), mode (0 = plain, else checked).
// Env::call(id, w) ids, one per call site of the routine:
//  1 0x69410(eax = header word, a2)  2 0x69350(name buffer, cache root)  3 0x69970(name buffer, cache root)
//  4 0x6D6D0(list, 0, 0)  5/9 0x6DD30 fill (eax out word, ecx node, edx sub | c << 16, a2, header, a1; on return w[0] = record word 13)
//  6/10 D2CMP #10005(rec, 1, 1)  8/12 D2CMP #10005(rec, 0, 0)  7/11 D2CMP #10103(rec)  13 fatal(line)
// 5/6/7/8 are the sub-record loop, 9..12 the single-record path taken when the header count is zero.
#include <cstdint>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;

struct D2cSpawnEnv {
    u32 (*rd8)(u32 addr);
    u32 (*call)(u32 id, u32* w);
};

extern "C" {
EMSCRIPTEN_KEEPALIVE void d2c_69cc0(const D2cSpawnEnv* e, u32 a1, u32 a2, u32 hdr, u32 mode) {
    u32 w[6];
    auto C = [&](u32 id, u32 a, u32 b, u32 c2, u32 d, u32 f, u32 g) { w[0] = a; w[1] = b; w[2] = c2; w[3] = d; w[4] = f; w[5] = g; return e->call(id, w); };
    u32 hw = e->rd8(hdr) | (e->rd8(hdr + 1) << 8);
    u32 outv = C(1, (u32)(i32)(int16_t)hw, a2, 0, 0, 0, 0);
    u32 node = C(2, 0x6fbc44c8, 0, 0, 0, 0, 0);
    if (!node) {
        node = C(3, 0x6fbc44c8, 0, 0, 0, 0, 0);
        if (!node) { C(13, 0xec3, 0, 0, 0, 0, 0); return; }
    }
    u32 list = e->rd8(node + 0x14) | (e->rd8(node + 0x15) << 8) | (e->rd8(node + 0x16) << 16) | (e->rd8(node + 0x17) << 24);
    if (!list) { C(13, 0xec9, 0, 0, 0, 0, 0); return; }
    u32 order = C(4, list, 0, 0, 0, 0, 0);
    if (!order) { C(13, 0xed0, 0, 0, 0, 0, 0); return; }
    u32 cnt = e->rd8(list);
    u32 b3 = e->rd8(hdr + 3);
    for (u32 i = 0; i < cnt; i++) {
        u32 c = e->rd8(order + i);
        if (c >= 16) { C(13, 0xed7, 0, 0, 0, 0, 0); return; }
        u32 n = b3 ? b3 : 1;
        u32 base = b3 ? 5 : 9;
        for (u32 sub = 0; sub < n; sub++) {
            u32 ok = C(base, outv, node, (b3 ? sub : 0) | (c << 16), a2, hdr, a1);
            u32 rec13 = w[0];
            if (!ok) continue;
            if (mode) {
                if (!C(base + 1, 0, c << 8, rec13, 1, 1, 0)) C(base + 2, 0, c << 8, 0, 0, 0, 0);
            } else {
                C(base + 3, 0, c << 8, rec13, 0, 0, 0);
            }
        }
    }
}
}

// Replay probe for the 3483 records: rec = the record's u32 words, out = {fatal, n, log[24][7]}.
namespace {
struct P4 { unsigned char fx[0x1000]; const u32* r; u32 n, fatal; u32* log; };
P4 g4;
u32 rd8_4(u32 a) { u32 o = a - 0xF1000000u; return o < 0x1000 ? g4.fx[o] : 0; }
u32 call4(u32 id, u32* w) {
    if (g4.n < 24) { u32* l = g4.log + g4.n * 7; l[0] = id; for (int i = 0; i < 6; i++) l[1 + i] = w[i]; }
    u32 k = g4.n & 15; g4.n++;
    const u32* r = g4.r;
    u32 ret = 0;
    switch (id) {
    case 1: ret = r[14]; break;
    case 2: ret = r[5] ? 0xF1000100u : 0; break;
    case 3: ret = r[6] ? 0xF1000100u : 0; break;
    case 4: ret = r[9] ? 0xF1000300u : 0; break;
    case 5: case 9: ret = r[15 + k]; w[0] = r[47 + k]; break;
    case 6: case 8: case 10: case 12: ret = r[31 + k]; break;
    case 13: g4.fatal = w[0]; break;
    default: break;
    }
    return ret;
}
}

extern "C" {
EMSCRIPTEN_KEEPALIVE void d2_client_69cc0(const u32* r, u32* out) {
    for (auto& b : g4.fx) b = 0;
    auto W = [&](u32 off, u32 v) { *(u32*)(g4.fx + off) = v; };
    W(0x40, (r[3] << 24) | (r[2] & 0xFFFF));
    W(0x100 + 0x14, r[7] ? 0xF1000200u : 0u);
    g4.fx[0x200] = (unsigned char)r[8];
    for (int i = 0; i < 16; i++) g4.fx[0x300 + i] = ((const unsigned char*)(r + 10))[i];
    g4.r = r; g4.n = g4.fatal = 0; g4.log = out + 2;
    for (int i = 0; i < 2 + 24 * 7; i++) out[i] = 0;
    D2cSpawnEnv e{ rd8_4, call4 };
    d2c_69cc0(&e, r[0], r[1], 0xF1000040u, r[4]);
    out[0] = g4.fatal; out[1] = g4.n;
}
}
