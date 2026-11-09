// 1.13c D2Client+0x7CC60 (stdcall, 1 stack word, ret 4): draw the animated scroll-in strip (scroin2/scroin3) for five slots.
// Written from the disassembly. Globals are reached through `at` (module RVA -> 32-bit address of the
// pinned bytes). The draw records (0x48 bytes) are stack locals in the caller frame: record 1 is cleared once and keeps word 0 = slot,
// +0x34 = the scroin2 cel; record 2 is cleared per draw. Each 10041 call (thunk 0xD096) is logged with its six stack words, then
// the caller frame from the slot word up to the end of record 1 (the block a `hookblk 0xd096 k 30` records).
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t M = 0xB2000000u;
}
struct D2C7CC60G {
    void* ctx; uint32_t (*at)(void* ctx, uint32_t rva); uint32_t now;   // `now`: GetTickCount
};
extern "C" void d2c_7cc60(uint32_t arg, const D2C7CC60G& g, const cl::Env& e) {
    auto G = [&](uint32_t rva) { return g.at(g.ctx, rva); };
    auto R = [&](uint32_t rva) { return e.R(G(rva)); };
    auto W = [&](uint32_t rva, uint32_t v) { e.W(G(rva), v); };
    if (R(0x11bbfc) == 0) return;
    // the caller frame from [esp+0x10]: slot, saved font, record 1 (18 words)
    uint32_t fr[20] = {};
    uint32_t* const rec1 = fr + 2;
    e.H(0x7c380);
    auto load = [&](uint32_t gv, const char* name) {   // sprintf("%s\\%s", "DATA\\GLOBAL", name) into a zeroed 0x104-byte local, then 0x2B420
        if (R(gv) != 0) return;
        uint32_t path[65] = {};
        char* p = (char*)path; const char* a = "DATA\\GLOBAL";
        uint32_t n = 0;
        while (*a) p[n++] = *a++;
        p[n++] = '\\';
        while (*name) p[n++] = *name++;
        W(gv, e.H(0x2b420, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)path));
        if (e.blk) e.blk(e.ctx, 0x2b420, (uint32_t)(uintptr_t)path, 8);
    };
    load(0x11bfac, "UI\\menu\\scroin2");
    load(0x11bfa8, "UI\\menu\\scroin3");
    if (arg == 0) return;
    fr[1] = e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 5);
    rec1[0x34 / 4] = R(0x11bfac);
    const uint32_t b03 = 0x113b03, b07 = 0x113b07;   // unaligned dwords
    if (R(b03) == 0) { W(b03, g.now); W(b07, 0); }
    else if (R(b03) + 0x32 < g.now) { W(b03, g.now); W(b07, R(b07) + 1); }
    for (uint32_t i = 0; i < 5; i++) {
        fr[0] = i;
        const uint32_t base = R(0xe1c90 + 4 * i), cur = R(b07);
        if (cur <= base) continue;
        const uint32_t d = cur - base;
        rec1[0] = i;
        uint32_t mode = 5;
        if (d == 1) { e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 0xa6f); mode = 0; }
        else if ((int32_t)d < 5) mode = 0;
        else if ((int32_t)d < 10) mode = 1;
        else if ((int32_t)d < 0xf) mode = 2;
        const uint32_t s = R(0x113950 + 2 * i) & 0xffff;
        if (s >= 6) continue;
        if (s == 5) { const uint32_t m = e.H(0xbf92, 0x164d); e.H(0xbf80, M | 0xd359f, m); e.H(0x3664, 0xffffffffu); return; }
        auto draw = [&](uint32_t rec, int32_t x, int32_t y, uint32_t md) {
            e.H(0xd096, rec, (uint32_t)x, (uint32_t)y, 0xffffffffu, md);
            uint32_t k[30] = { rec, (uint32_t)x, (uint32_t)y, 0xffffffffu, md, 0, 0, 0, 0, 0 };   // words 6..9: saved registers + residue (masked)
            for (int j = 0; j < 20; j++) k[10 + j] = fr[j];
            if (e.blk) e.blk(e.ctx, 0xd096, (uint32_t)(uintptr_t)k, 30);
        };
        const int32_t ox = (int32_t)R(0x11b9a0), oy = (int32_t)R(0x11b9a4);
        draw((uint32_t)(uintptr_t)rec1, (int32_t)R(0xe1c40 + 8 * s) + ox, (int32_t)R(0xe1c44 + 8 * s) - oy, mode);
        uint32_t rec2[18] = {};
        rec2[0x34 / 4] = R(0x11bfa8);
        rec2[0] = (int32_t)d >= 0x15 ? 0 : d;
        draw((uint32_t)(uintptr_t)rec2, (int32_t)R(0xe1c68 + 8 * s) + ox, (int32_t)R(0xe1c6c + 8 * s) - oy, 3);
    }
    e.H(0xd3a8, 0, 0, 0, 0, 0, 0, fr[1]);
}
