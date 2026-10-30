// 1.13c D2Client+0x6B300 -- COF component submit (env-seam port; every callee and memory read goes through the Env).
// Stack ABI (ret 0x1c): a0 unit, a1 type, a2 rand-shift-8, a3..a5 node 8/c/10 (a3 -> +0x38 for type 8),
// a6 rand-shift-4; EAX = component selector.
// Env::call(id, w) ids, one per call site of the routine:
//  1 #10227  2 0x1510(unit id, 2)  3 0x19130(unit, sel)  4 0x19660(sel)  5 alloc(0xa8, 0xc48, 0, tag)  6 0x1E9C0(node, unit)
//  7/13/14 0x1380(g+0x20, v) at the type-6 / a2 / a6 sites  8 #10172(unit)  9 0x58C20(5 stack args, ebx 1)
//  10 0x58BE0(ecx result, eax row+0x74)  11 fatal(line)  12 0x18AE0(node)
#include <cstdint>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;

struct D2cCofEnv {
    u32 (*rd32)(u32 addr);
    void (*wr32)(u32 addr, u32 v);
    u32 (*call)(u32 id, const u32* w);
};

extern "C" {
EMSCRIPTEN_KEEPALIVE void d2c_6b300(const D2cCofEnv* e, u32 sel, u32 unit, u32 type, u32 a2, u32 a3,
                                    u32 a4, u32 a5, u32 a6) {
    u32 w[6];
    auto C = [&](u32 id, u32 a, u32 b, u32 c2, u32 d, u32 f, u32 g2) { w[0] = a; w[1] = b; w[2] = c2; w[3] = d; w[4] = f; w[5] = g2; return e->call(id, w); };
    if ((i32)sel < 0) return;
    u32 dt = e->rd32(e->rd32(0x6fb7e474));
    if ((i32)sel >= (i32)e->rd32(dt + 0xbc0)) return;
    u32 row = sel * 0x84 + e->rd32(dt + 0xbbc);
    if (!row) return;
    if (!C(1, 0, 0, 0, 0, 0, 0) && (int16_t)(e->rd32(row + 0x40) >> 16) >= 100) return;
    u32 g = e->rd32(0x6fbcbbfc);
    if (unit) {
        if ((e->rd32(unit + 0xc8) >> 0x12) & 1) return;
        if (e->rd32(unit) == 1 && C(2, e->rd32(unit + 4), 2, 0, 0, 0, 0)) return;
    }
    if (type == 8 && C(3, unit, sel, 0, 0, 0, 0)) return;
    if ((i32)sel < 0x8c || (sel > 0x8e && sel != 0x9e)) C(4, sel, 0, 0, 0, 0, 0);
    u32 n = C(5, 0xa8, 0xc48, 0, 0x6fb84fa4, 0, 0);
    if (!n) { C(11, 0xc49, 0, 0, 0, 0, 0); return; }
    for (u32 i = 0; i < 0xa8; i += 4) e->wr32(n + i, 0);
    e->wr32(n, type); e->wr32(n + 4, sel);
    u32 n1c = C(6, n, unit, 0, 0, 0, 0);
    e->wr32(n + 0x1c, n1c);
    if (type == 2) e->wr32(n + 8, 1);
    else if (type == 6) e->wr32(n + 0x18, C(7, g + 0x20, n1c, 0, 0, 0, 0));
    else { e->wr32(n + 8, a3); e->wr32(n + 0xc, a4); e->wr32(n + 0x10, a5); }
    e->wr32(n + 0x20, e->rd32(row + 0x54));
    u32 y = e->rd32(row + 0x58);
    e->wr32(n + 0x24, y);
    switch ((i32)C(8, unit, 0, 0, 0, 0, 0)) {
    case -1: y += 0x4b; break;
    case 0: y += e->rd32(row + 0x5c); break;
    case 1: y += e->rd32(row + 0x60); break;
    case 2: y += e->rd32(row + 0x64); break;
    case 3: y += e->rd32(row + 0x68); break;
    default: break;
    }
    e->wr32(n + 0x24, y);
    if (e->rd32(row + 0x74)) {
        u32 r = C(9, e->rd32(row + 0x70), 0xff, e->rd32(row + 0x7d) & 0xff, e->rd32(row + 0x7e) & 0xff, e->rd32(row + 0x7f) & 0xff, 1);
        e->wr32(n + 0x40, r);
        if (e->rd32(row + 0x74) != e->rd32(row + 0x70)) C(10, r, e->rd32(row + 0x74), 0, 0, 0, 0);
    }
    u32 pool = unit ? e->rd32(unit + 0x54) : 0;
    if (!pool) { C(11, 0xc7a, 0, 0, 0, 0, 0); return; }
    e->wr32(n + 0x34, e->rd32(row + 0x48) & 0xff);
    e->wr32(n + 0xa4, e->rd32(pool + 0x2c));
    e->wr32(pool + 0x2c, n);
    if (type == 8) { e->wr32(n + 0x38, a3); C(12, n, 0, 0, 0, 0, 0); }
    if (a2) e->wr32(n + 0x18, C(13, g + 0x20, a2 << 8, 0, 0, 0, 0));
    u32 f = e->rd32(row + 0x6c) << 4;
    e->wr32(n + 0x14, f);
    if (a6) f += C(14, g + 0x20, a6 << 4, 0, 0, 0, 0);
    e->wr32(n + 0x14, f);
    e->wr32(n + 0x28, e->rd32(row + 0x78));
}

// Replay probe for the 3482 records: rec = the record's u32 words, out = {fatal, n, poolHeadOut, node[42], log[16][7]}.
namespace {
struct P3 { unsigned char fx[0x2000]; const u32* r; u32 n, fatal; u32* log; };
P3 g3;
u32 rd3(u32 a) {
    if (a == 0x6fb7e474u) return 0xF1000400u;
    if (a == 0x6fbcbbfcu) return g3.r[15];
    u32 o = a - 0xF1000000u; return o < 0x1FFD ? *(u32*)(g3.fx + o) : 0;
}
void wr3(u32 a, u32 v) { u32 o = a - 0xF1000000u; if (o < 0x1FFD) *(u32*)(g3.fx + o) = v; }
u32 call3(u32 id, const u32* w) {
    if (g3.n < 16) { u32* l = g3.log + g3.n * 7; l[0] = id; for (int i = 0; i < 6; i++) l[1 + i] = w[i]; }
    g3.n++;
    u32 ret = g3.r[49 + id];
    if (id == 5) ret = ret ? 0xF1000200u : 0;
    if (id == 11) g3.fatal = w[0];
    return ret;
}
}
EMSCRIPTEN_KEEPALIVE void d2_client_6b300(const u32* r, u32* out) {
    for (auto& b : g3.fx) b = 0;
    auto W = [&](u32 off, u32 v) { *(u32*)(g3.fx + off) = v; };
    if (r[0]) { W(0, r[1]); W(4, r[2]); W(0xC8, r[3]); W(0x54, r[4] ? 0xF1000100u : 0u); }
    W(0x100 + 0x2c, r[5]);
    W(0x400, 0xF1000800u); W(0x800 + 0xbc0, r[13]);
    u32 base = r[14] ? (u32)(0 - r[6] * 0x84u) : 0xF1000300u - r[6] * 0x84u;
    W(0x800 + 0xbbc, base);
    for (int i = 0; i < 33; i++) W(0x300 + i * 4, r[16 + i]);
    g3.r = r; g3.n = g3.fatal = 0; g3.log = out + 45;
    for (int i = 0; i < 45 + 16 * 7; i++) out[i] = 0;
    D2cCofEnv e{ rd3, wr3, call3 };
    d2c_6b300(&e, r[6], r[0] ? 0xF1000000u : 0u, r[7], r[8], r[9], r[10], r[11], r[12]);
    out[0] = g3.fatal; out[1] = g3.n;
    out[2] = (r[0] && r[4]) ? *(u32*)(g3.fx + 0x100 + 0x2c) : 0;
    for (int i = 0; i < 42; i++) out[3 + i] = *(u32*)(g3.fx + 0x200 + i * 4);
}
}
