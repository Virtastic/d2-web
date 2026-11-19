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
    uint8_t b330_bbb330 = 0, b331_bbb331 = 0;
    virtual uint32_t fn5(uint32_t rva, uint32_t a, uint32_t b, uint32_t c, uint32_t, uint32_t) { return fn(rva, a, b, c); }
};

// 1.13c D2Client+0x8E8E0: stdcall, one stack word p (ret 4). 10217 takes (item, slot index); 0x8C760 takes EBX = item, EDI = a stack
// local, stack p; 10244 (item, p, 0, 0, 0, 0). 0x68970: EAX = top, ECX = left, EDX = 0, stack (rect byte +0x10, byte +0x11,
// byte 0x10B331 when all three calls succeeded, else byte 0x10B330).
void sub_8E8E0(Env& e, uint32_t p) {
    uint32_t it = e.ord(11017, e.rd32(e.player_cbbfc + 0x60));
    if (!it) return;
    for (int i = 0; i < 11; ++i) {
        const int32_t* r = &e.rects_cbd80[i * 5];
        if (0 < r[0] && r[0] <= e.mouseX_cb828 && e.mouseX_cb828 <= r[1] &&
            r[2] <= e.mouseY_cb824 && e.mouseY_cb824 <= r[3]) {
            e.ord(10114, it, 0, 0, 0x6da);
            const bool hit = e.ord(10217, it, (uint32_t)i) && e.fn(0x8c760, it, p) && e.ord(10244, it, p);
            if (r[0] >= 0 && r[0] < e.limX_a9e14 && r[2] >= 0 && r[2] < e.limY_a9e18) {
                const uint32_t w = (uint32_t)r[4];
                e.fn5(0x68970, (uint32_t)r[2], (uint32_t)r[0], w & 0xff, (w >> 8) & 0xff, hit ? e.b331_bbb331 : e.b330_bbb330);
            }
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

// 1.13c D2Client+0x908C0 (per-screen panel rect refresh); 0x119721 is read as a dword
void sub_908C0(Env& e, int32_t& cb99c, int32_t& c3424, int32_t& c20, int32_t& c24, int32_t pool, uint32_t pl2, uint32_t flag9721, uint32_t id971d, uint32_t cc854, int32_t* regionsOut) {
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
