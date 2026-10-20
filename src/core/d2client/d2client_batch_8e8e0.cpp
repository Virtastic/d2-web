// src/core/d2client/d2client_batch_8e8e0.cpp -- twelve UNMAPPED 1.13c D2Client.dll rows (alpha lane),
// reimplemented from build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000.
// Env-seam: globals and unrecovered imports (ordinals, FUN_ calls) are Env callbacks; the control flow
// and arithmetic are the decompile's. Where the decompiler lost a register argument (string ids, colour
// targets) the callback takes the call-site tag instead; the vector pass pins these.
// Strings are UTF-16 (D2Lang Unicode), NUL-terminated.
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <emscripten.h>

namespace d2client_8e8e0 {
using W = uint16_t;
static void wcpy(W* d, const W* s) { while ((*d++ = *s++)) {} }
static void wcat(W* d, const W* s) { while (*d) ++d; wcpy(d, s); }
static const W kEmpty[1] = {0};
// Ordinal_578 (sprintf) + Unicode::win2Unicode, fused: "%d" widened into d.
static void wnum(W* d, int32_t v) { char b[16]; std::snprintf(b, sizeof b, "%d", v); for (const char* p = b; (*d++ = (uint8_t)*p++);) {} }

struct Env {
    virtual ~Env() = default;
    // globals (D2Client .data RVA in the name)
    int32_t mouseX_cb828 = 0, mouseY_cb824 = 0, limX_a9e14 = 0, limY_a9e18 = 0;
    int32_t state_cbc34 = 0, c74 = 0, c84 = 0, c88 = 0, c28 = 0, c2c = 0;
    int32_t scrW_8bc48 = 0, scrH_8bc4c = 0, mode_cc414 = 0, sub_cb864 = 0, cb834 = 0, cb858 = 0, cb854 = 0;
    int32_t layoutBase_cb9a0 = 0, layoutY_cb9a4 = 0, tab_911c4 = 0, tabN_911c8 = 0;
    int32_t b2a4 = 0, b408 = 0, b40c = 0, b430 = 0, b414 = 0;
    int32_t entries_bb238[5 * 16] = {};   // 5-int records, [0]=active, [1]=0x6fbb23c, [2].lo16=0x6fbb240
    int32_t tab_911d0[256] = {};
    uint32_t player_cbbfc = 0;
    int32_t rects_cbd80[5 * 11] = {};      // 11 records x 5 ints (0x14), fields l,r,t,b
    // imports
    virtual uint32_t ord(uint32_t n, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0, uint32_t e = 0) = 0;
    virtual uint32_t fn(uint32_t rva, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0) = 0;  // FUN_6fb<rva>
    virtual const W* txt(uint32_t site) = 0;                  // Ordinal_10003 at a call-site tag
    virtual void color(uint8_t c, W* dst) = 0;                // FUN_6fb6f5f0 colour escape into dst
    [[noreturn]] virtual void fatal(uint32_t msg) = 0;        // Ordinal_10265 + 10024 + _exit(-1)
    virtual void packet(const uint8_t* p, unsigned n) = 0;    // FUN_6fac43e0
    virtual uint8_t rd8(uint32_t addr) = 0;                   // guest byte read (data-table rows)
    virtual uint32_t rd32(uint32_t addr) = 0;
    virtual void wr32(uint32_t addr, uint32_t v) = 0;
    virtual uint32_t dataTables() = 0;                        // *sgptDataTables
};

// 1.13c D2Client+0x8E8E0
void sub_8E8E0(Env& e, uint32_t edi, uint32_t retaddr) {
    uint32_t it = e.ord(11017, e.rd32(e.player_cbbfc + 0x60));
    if (!it) return;
    for (int i = 0; i < 11; ++i) {
        const int32_t* r = &e.rects_cbd80[i * 5];
        if (0 < r[0] && r[0] <= e.mouseX_cb828 && e.mouseX_cb828 <= r[1] &&
            r[2] <= e.mouseY_cb824 && e.mouseY_cb824 <= r[3]) {
            e.ord(10114, it, 0, 0, 0x6da);
            uint32_t a = e.ord(10217, it, edi);
            bool hit = a && e.fn(0x3c760, retaddr) && e.ord(10244, it, retaddr);
            (void)hit;  // both arms of the decompile make the same bounds test and the same call
            if (r[0] >= 0 && r[0] < e.limX_a9e14 && r[2] >= 0 && r[2] < e.limY_a9e18) e.fn(0x18970, r[0]);
        }
    }
}

// 1.13c D2Client+0x8F140
void sub_8F140(Env& e) {
    if (e.state_cbc34 == 0xc || e.state_cbc34 == 0xd) {
        e.state_cbc34 = 0;
        e.fn(0xc2790, 0x19, 1, 0);
        const uint8_t pk[7] = {'O', 0x12, 0, 0, 0, 0, 0};
        e.packet(pk, 7);
    }
    int32_t* slot[2] = {&e.c74, &e.c84};
    for (int32_t* s : slot) {
        int32_t v = *s;
        if (!v) continue;
        e.ord(10065, v);
        e.ord(10043, 0x7a, 0);
        *s = 0;
    }
}

// 1.13c D2Client+0x8F260
void sub_8F260(Env& e) {
    int32_t y0 = e.layoutY_cb9a4 - 0x6d + e.scrH_8bc4c;
    if (!(e.b408 <= e.b2a4 && e.b2a4 <= e.b40c && y0 <= e.b430 && e.b430 <= e.b414)) return;
    int i = 0;
    for (; i < e.tabN_911c8; ++i) {
        if (e.entries_bb238[i * 5] != 1) continue;
        int32_t x = e.tab_911d0[i + e.tab_911c4 * 4] + e.layoutBase_cb9a0;
        if (e.b2a4 >= x || e.b2a4 >= x + 0x2d) continue;
        if (e.b430 >= y0 || e.layoutY_cb9a4 - 0x41 + e.scrH_8bc4c <= e.b430) continue;
        break;
    }
    if (i >= e.tabN_911c8) return;
    int32_t ty = e.layoutY_cb9a4 - 100 + e.scrH_8bc4c;
    if ((int16_t)(e.entries_bb238[i * 5 + 2] & 0xffff) == 0x276f) {
        uint32_t pl = e.player_cbbfc;
        uint32_t rec = e.fn(0x46150);  // FUN_6faf6150 -> int*
        if (pl && rec) {
            W buf[512];
            uint32_t id = e.ord(10071, 0, pl, e.rd32(rec + 4));
            e.ord(10085, ty, 0, 0);
            (void)buf; (void)id;       // Unicode::sprintf(0x100, buf, Ordinal_10003(id)) feeds 10085
        }
    } else {
        e.ord(10003, ty, 0);
        e.ord(10085, ty, 0, 0);
    }
}

// 1.13c D2Client+0x8F630
void sub_8F630(Env& e) {
    if (e.state_cbc34 != 7) e.fatal(e.ord(10265, 0x1ec2));
    e.fn(0x8d300);
}

// 1.13c D2Client+0x8F660
void sub_8F660(Env& e) {
    e.c88 = -1;
    int32_t found = e.c88;
    for (int i = 0; i < e.tabN_911c8; ++i) {
        found = i;
        if (e.entries_bb238[i * 5 + 1] != 0) break;
        found = e.c88;
    }
    e.c88 = found;
    e.entries_bb238[1] = 0; e.entries_bb238[6] = 0; e.entries_bb238[11] = 0; e.entries_bb238[16] = 0;
    e.state_cbc34 = 1;
    e.cb834 = (int32_t)e.ord(11017, e.rd32(e.player_cbbfc + 0x60));
    e.cb858 = 0; e.cb854 = 5; e.sub_cb864 = 1;
    if (e.cb834) e.sub_cb864 = 4;
}

// 1.13c D2Client+0x8F880 (param_1 = stack arg, edi = cursor Y, esi = struct whose +0x18 is set)
void sub_8F880(Env& e, int32_t p1, int32_t edi, uint32_t esi) {
    if (e.ord(10031) != 2) return;
    if (e.fn(0x26320) != 0 || p1 > e.scrH_8bc4c - 0x30) return;
    if (e.mode_cc414 == 1 && e.scrW_8bc48 / 2 > edi) return;
    if (e.sub_cb864 == 6) e.fn(0xbeb90);
    if (e.mode_cc414 == 3 && e.state_cbc34 == 0 && e.scrW_8bc48 / 2 > edi) return;
    e.wr32(esi + 0x18, 1);
    e.c28 = 0; e.c2c = 0;
    e.ord(511);
}

// 1.13c D2Client+0x8F9E0 (tooltip name line; out = tooltip buffer, unaff_EBP = trailing text)
void sub_8F9E0(Env& e, W* out, const W* tail, const W* sep) {
    W a[512] = {};
    e.txt(0); e.txt(1); e.txt(2);
    uint32_t pl = e.player_cbbfc;
    int32_t lvl = (int32_t)e.ord(10592, pl);
    bool hi = e.ord(10379) != 0;
    if (lvl < 0x1c && 9 < lvl && e.ord(10744) == 0) e.ord(10744);
    W* dst = out;
    for (int i = 0; i < 0x1c; ++i) {            // 28 six-byte entries at 0x6fb90ef0
        if (e.ord(10744, i)) {
            wcpy(out, e.txt(3)); wcat(out, sep); wcat(out, kEmpty); wcat(out, sep);
            break;
        }
    }
    wcpy(a, e.txt(4));
    if (hi) e.color(3, a);
    wcat(dst, a);
    wcat(dst, tail);
}

// 1.13c D2Client+0x8FB80 (item damage/defense line; param_1 = item, out = ESI buffer)
void sub_8FB80(Env& e, uint32_t item, W* out, uint32_t ebx) {
    if (!item) return;
    uint32_t base = e.ord(11160, item);
    uint32_t it = (base && base != item) ? base : item;
    e.ord(10164, it, 1);
    int32_t v = (int32_t)e.ord(10935, it);
    if (!base) e.ord(11127, it); else e.ord(10164, it);
    bool diff = e.ord(10587, 0x1f, 0) != (uint32_t)v;
    uint32_t dt = e.dataTables();
    uint32_t p = e.ord(10973, item, 0x10c);
    if (p && 0x10c < (int32_t)e.rd32(dt + 0xbd4) && (int32_t)e.rd32(dt + 0xbcc) != -0x15330 && e.rd32(ebx + 0x1c)) {
        int32_t d = (int32_t)e.ord(10449, p, e.ord(11044, e.rd32(ebx + 0x1c)), 0);
        v += d; if (d) diff = true;
    }
    p = e.ord(10973, item, 0x10d, 0);
    if (p && 0x10d < (int32_t)e.rd32(dt + 0xbd4) && (int32_t)e.rd32(dt + 0xbcc) != -0x15474 && e.rd32(ebx + 0x1c)) {
        int32_t d = (int32_t)e.ord(10449, p, e.ord(11044, e.rd32(ebx + 0x1c)), 0);
        v += (int32_t)e.fn(0x14a0, 100, v);
        if (d) diff = true;
    }
    W num[16];
    wcpy(out, e.txt(0)); wcat(out, kEmpty);
    wnum(num, v);
    if (diff) e.color(3, out);
    wcat(out, num);
    wcat(out, e.txt(1));
}

// 1.13c D2Client+0x8FDC0 (defense/resist percent line into param_1)
void sub_8FDC0(Env& e, W* out, uint32_t ctx) {
    int32_t v = (int32_t)e.ord(10973);
    uint32_t pl = e.player_cbbfc;
    bool run = true;
    if (pl) {
        uint32_t dt = e.dataTables();
        int32_t cls = (int32_t)e.rd32(pl + 4);
        uint32_t row = (cls < 0 || (int32_t)e.rd32(dt + 0xbc8) <= cls) ? 0 : cls * 0xc4 + e.rd32(dt + 0xbc4);
        v += row ? e.rd8(row + 0x49) : 0;
        uint32_t x = e.ord(10630);
        if (x && e.ord(10494, pl, 0x65)) v += (int32_t)e.ord(10832, e.ord(10306, pl, x, 1), 0x75);
        if (v > 0x4b) v = 0x4b;
    }
    if (v == 0 && !(pl && v > 0x4b)) { run = false; }
    if (!run) return;
    W num[32] = {}, buf[512] = {};
    char c[32]; std::snprintf(c, sizeof c, "%d%%\n", v);
    for (int i = 0; (num[i] = (uint8_t)c[i]); ++i) {}
    if ((int32_t)e.rd8(ctx + 0x111) < v) e.color(3, num);
    wcpy(buf, e.txt(0));
    e.color(0, buf);
    wcpy(out, buf);
    wcat(out, num);
}

// 1.13c D2Client+0x8FF20 (item damage range, two-hand/throw variants; param_1 = item, out = tooltip)
void sub_8FF20(Env& e, uint32_t item, W* out, W* out2, const W* sp) {
    W acc[512] = {}, num[36], num2[36], tmp[512] = {};
    uint32_t pl = e.player_cbbfc;
    e.txt(0);
    int32_t lo = 0, hi = 0, mn = 0, mx = 0, bonus = 0;
    if (e.ord(10744, pl, 0x26)) {                       // thrown / ethereal-style single range
        uint32_t it = e.ord(10321);
        wcpy(tmp, e.txt(1)); e.color(0, tmp);
        e.ord(10205); e.ord(10532);
        if ((uint8_t)e.ord(10557, it) == 5) e.ord(10242, 0, pl, it, 1);
        lo = (int32_t)e.ord(10040); hi = (int32_t)e.ord(10256, 0, pl, it, 1);
        int32_t a = lo >> 8, b = hi >> 8; if (b < a) b = a;
        wnum(num, a); e.color(0, num);
        wcat(acc, sp); wcat(acc, num);
        if (a != b) { wcat(acc, sp); wcat(acc, e.txt(2)); wnum(num, b); e.color(0, num); wcat(acc, sp); wcat(acc, num); }
        wcpy(out, acc);
        return;
    }
    bool two = e.ord(10364) != 0;
    if (!two) {
        if (e.ord(10326)) e.fn(0x8dad0, pl, 0x17, 0x18); else e.fn(0x8dad0, pl, 0x15, 0x16);
        mn = (int32_t)e.fn(0x8dad0 + 1); mx = (mn <= mx + 1) ? mx + 1 : mn;
        wcpy(out, e.txt(3)); wnum(num, mn); wcat(out, sp); if (bonus) e.color(3, out);
        wcat(out, num); wcat(out, sp); wcat(out, e.txt(4)); wnum(num2, mx);
        wcat(out, sp); wcat(out, num2); wcat(out, kEmpty);
    } else {
        e.fn(0x8dad0, pl, 0x17, 0x18);
        W one[256] = {}, two_[256] = {};
        wcpy(one, e.txt(5)); wcat(one, sp); wnum(num, mn); if (lo) e.color(3, num);
        wcat(one, num); wcat(one, sp); wcat(one, e.txt(6)); wnum(num2, mx); wcat(one, sp); wcat(one, num2);
        e.fn(0x8dad0, pl, 0x15, 0x16);
        wcpy(two_, e.txt(7)); wcat(two_, sp); wnum(num, mn); if (item) e.color(3, num);
        wcat(two_, num); wcat(two_, sp); wcat(two_, e.txt(8)); wnum(num2, mx);
        wcat(two_, sp); wcat(two_, num2); wcat(two_, kEmpty);
        e.color(0, two_); e.color(0, one);
        wcpy(out2, one); wcat(out2, two_);
    }
    bool boost = e.ord(10379) || e.ord(10379, item, 0x11) || e.ord(10379, item, 0x9f) || e.ord(10379, item, 0xa0);
    if (boost) mn = 1;
    if (e.ord(10711, item)) {
        W t[512] = {};
        wcpy(t, e.txt(9)); e.color(0, t);
        e.fn(0x8dad0, pl, 0x9f, 0xa0);
        wnum(num, mn);
        uint8_t col = boost ? 3 : 0;
        e.color(col, num);
        wcat(out, sp); wcat(out, t); wcat(out, num); wcat(out, e.txt(10));
        wnum(num, mx); e.color(col, num);
        wcat(out, sp); wcat(out, num); wcat(out, sp); wcat(out, kEmpty);
    }
}

// 1.13c D2Client+0x90770 (item quantity/durability line; out = ESI buffer)
void sub_90770(Env& e, uint32_t item, W* out) {
    W num[16] = {};
    const W* sp = e.txt(0);
    const W* sp2 = e.txt(1);
    if (!e.ord(10865, item)) return;
    if (e.ord(11116, item) <= 0) return;
    if (e.ord(10711, item)) return;
    bool cap = e.ord(10379, item) != 0;
    wcpy(out, e.txt(2));
    wnum(num, (int32_t)e.ord(10973, item, 0x48));
    wcat(out, sp); wcat(out, num); wcat(out, sp); wcat(out, e.txt(3)); wcat(out, sp);
    wnum(num, (int32_t)e.ord(11116, item));
    if (sp2 && sp2[0]) e.color(3, num);
    (void)cap;
    wcat(out, num); wcat(out, kEmpty);
}

// 1.13c D2Client+0x908C0 (per-screen panel rect refresh)
void sub_908C0(Env& e, int32_t& cb99c, int32_t& c3424, int32_t& c20, int32_t& c24, int32_t pool, uint32_t pl2, uint8_t flag9721, uint32_t id971d, uint32_t cc854, int32_t* regionsOut) {
    if (cb99c == c3424) {
        if (c20 == 0) goto fill;
    } else {
        c3424 = cb99c; c20 = 0; c24 = 0;
    fill:
        uint32_t k = e.ord(10584, e.player_cbbfc, 0, cc854);
        e.ord(11012, k, cb99c, 0x6fbbb2f8); e.ord(10760, k, cb99c, 0x6fbbb3e0);
        static const uint32_t m[10] = {0x6fbcbcf0, 0x6fbcbcdc, 0x6fbcbd04, 0x6fbcbcb4, 0x6fbcbcc8,
                                       0x6fbcbd18, 0x6fbcbd2c, 0x6fbcbd40, 0x6fbcbd54, 0x6fbcbd68};
        static const uint32_t n[10] = {0x6fbcbdd0, 0x6fbcbdbc, 0x6fbcbde4, 0x6fbcbd94, 0x6fbcbda8,
                                       0x6fbcbdf8, 0x6fbcbe0c, 0x6fbcbe20, 0x6fbcbe34, 0x6fbcbe48};
        for (uint32_t i = 0; i < 10; ++i) e.ord(10701, k, cb99c, m[i], i);
        e.ord(11012, 0xd, cb99c, 0x6fbbb360); e.ord(10760, 0xd, cb99c, 0x6fbbb418);
        for (uint32_t i = 0; i < 10; ++i) e.ord(10701, 0xd, cb99c, n[i], i);
        static const uint32_t t[7][3] = {{6, 0x6fbbb338, 0x6fbbb370}, {7, 0x6fbbb2e8, 0x6fbbb288}, {8, 0x6fbbb2c0, 0x6fbbb398}, {0xc, 0x6fbbb34c, 0x6fbbb2d0},
                                         {9, 0x6fbbb388, 0x6fbbb3c8}, {10, 0x6fbbb3f8, 0x6fbbb308}, {0xb, 0x6fbbb320, 0x6fbbb2a8}};
        for (auto& r : t) { e.ord(11012, r[0], cb99c, r[1]); e.ord(10760, r[0], cb99c, r[2]); }
        c20 = 1;
    }
    (void)pool; (void)pl2; (void)regionsOut;
    if (c24 == 0 && e.state_cbc34 != 0 && flag9721 != 0) {
        uint32_t u = e.fn(0, id971d & 0x7f);       // bucket head at 0x6fbba808[id & 0x7f]
        while (u && e.rd32(u + 12) != id971d) u = e.rd32(u + 0xe4);
        if (!u) return;
        if (e.rd32(u) != 1) e.fatal(e.ord(10265, 0x5a));
        uint32_t k = e.ord(10584, u, 0, cc854);
        e.ord(11012, k, cb99c, 0x6fbbb408); e.ord(10760, k, cb99c, 0x6fbbb3b0);
        c24 = 1;
    }
}
}  // namespace d2client_8e8e0
