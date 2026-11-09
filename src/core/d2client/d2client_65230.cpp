// 1.13c D2Client+0x65230 (no arguments, plain ret): the options-panel per-frame draw. Written from the disassembly.
// Globals (module RVAs, reached through `at`): 0x11C05C header pointer {count, rowH, top, titleY}, 0x11C060 entry table pointer
// (stride 0x550: +0 kind, +4 hide flag, +8 row y out, +0x110 state callback, +0x11C pre-draw callback, +0x124 sub-index,
// +0x53C cel, +0x540 sub-cel table), 0x11C058 selected row, 0x11C064 end-cap frame (0..7), 0x11C068 mode, 0x11C07C left offset,
// 0x11B824/0x11B828 cursor (latched in 0x123454/0x123458), 0x11B9C0 end-cap cel, 0xDBC48/0xDBC4C screen W/H, 0x123404 last tick.
// The table callbacks are code pointers in the table: the call goes to their low 24 bits (the capture's recorder stubs).
// Callees: 0x64080 (EBX = cursor x, returns a row or -1), 0x64FD0 (ECX = cursor x, stdcall cursor y), 0xBF8C (#10227),
// 0xD372 (#10138: ECX = cel, EDX = x; y, frame, state, -1), 0x63BF0 (ECX = row y; entry, state, cel == 0), 0xD096 (#10041: the
// 18-dword end-cap record, x, y, -1, 5, 0). The null-header (0x6BE) and null-table (0x6BF) exits do not return and are kept.
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t M = 0xB2000000u;
}
struct D2C65230G {
    void* ctx; uint32_t (*at)(void* ctx, uint32_t rva); uint32_t now;
};
extern "C" void d2c_65230(const D2C65230G& g, const cl::Env& e) {
    auto G = [&](uint32_t rva) { return g.at(g.ctx, rva); };
    auto R = [&](uint32_t rva) { return e.R(G(rva)); };
    auto W = [&](uint32_t rva, uint32_t v) { e.W(G(rva), v); };
    auto fatal = [&](uint32_t line) { const uint32_t m = e.H(0xbf92, line); e.H(0xbf80, M | 0xd359f, m); e.H(0x3664, 0xffffffffu); };
    if (R(0x11c05c) == 0) { fatal(0x6be); return; }
    if (R(0x11c060) == 0) { fatal(0x6bf); return; }
    uint32_t hdr = R(0x11c05c);
    const int32_t A = ((int32_t)R(0xdbc4c) - 0x50) / 2 - ((int32_t)e.R(hdr + 4) * (int32_t)e.R(hdr)) / 2;
    const uint32_t cx = R(0x11b824), cy = R(0x11b828);
    if (R(0x123454) != cx || R(0x123458) != cy) {
        const uint32_t mode = R(0x11c068);
        W(0x123454, cx);
        W(0x123458, cy);
        if (mode != 0) e.H(0x64fd0, cy, 0, 0, 0, 0, 0, cx);
        else {
            const uint32_t r = e.H(0x64080, 0, 0, 0, 0, 0, 0, 0, 0, cx);
            if (r != 0xffffffffu) W(0x11c058, r);
        }
    }
    hdr = R(0x11c05c);
    int32_t vis = 0;
    for (int32_t i = 0; i < (int32_t)e.R(hdr); hdr = R(0x11c05c), i++) {
        const uint32_t off = (uint32_t)i * 0x550;
        uint32_t tbl = R(0x11c060);
        const uint32_t hide = e.R(tbl + off + 4);
        e.W(tbl + off + 8, 0);
        if (hide != 0 && e.H(0xbf8c) == 0) continue;
        tbl = R(0x11c060);
        uint32_t on = 1;
        const uint32_t cb0 = e.R(tbl + off + 0x110);
        if (cb0 != 0) on = e.H(cb0 & 0x00ffffffu);
        tbl = R(0x11c060);
        const uint32_t st = on != 0 ? 5 : 1;
        const uint32_t cb1 = e.R(tbl + off + 0x11c);
        if (cb1 != 0) e.H(cb1 & 0x00ffffffu, 0, 0, 0, 0, 0, 0, tbl + off, 0);
        hdr = R(0x11c05c);
        const int32_t y = (int32_t)e.R(hdr + 4) * vis + A;
        const int32_t out = (int32_t)e.R(hdr + 8) + y;
        tbl = R(0x11c060);
        const uint32_t ent = tbl + off;
        const int32_t kind = (int32_t)e.R(ent);
        e.W(ent + 8, (uint32_t)y);
        const int32_t half = (int32_t)R(0xdbc48) / 2;
        switch (kind) {
        case -1: case 0:
            e.H(0xd372, (uint32_t)out, 1, st, 0xffffffffu, 0, 0, e.R(ent + 0x53c), (uint32_t)half);
            break;
        case 1: {
            e.H(0xd372, (uint32_t)out, 0, st, 0xffffffffu, 0, 0, e.R(ent + 0x53c), (uint32_t)(half - 0xe6));
            const uint32_t t2 = R(0x11c060);
            const uint32_t sub = e.R(t2 + 4 * (e.R(t2 + off + 0x124) + (uint32_t)i * 0x154) + 0x540);
            e.H(0xd372, (uint32_t)out, 2, st, 0xffffffffu, 0, 0, sub, (uint32_t)(half + 0xe6));
            break;
        }
        case 2: {
            const uint32_t cel = e.R(ent + 0x53c);
            if (cel != 0) e.H(0xd372, (uint32_t)out, 0, st, 0xffffffffu, 0, 0, cel, (uint32_t)(half - 0xe6));
            const uint32_t e2 = R(0x11c060) + off;
            e.H(0x63bf0, e2, st, e.R(e2 + 0x53c) == 0 ? 1u : 0u, 0, 0, 0, (uint32_t)y);
            break;
        }
        default: break;
        }
        vis++;
    }
    uint32_t rec[18] = {};
    rec[13] = R(0x11b9c0);
    const uint32_t an = R(0x11c064);
    rec[0] = an != 0 ? 8 - an : 0;
    const int32_t ry = (int32_t)e.R(R(0x11c058) * 0x550 + R(0x11c060) + 8) + (int32_t)e.R(R(0x11c05c) + 0xc);
    const int32_t half = (int32_t)R(0xdbc48) / 2;
    const uint32_t rp = (uint32_t)(uintptr_t)rec;
    e.H(0xd096, rp, (uint32_t)(half - (int32_t)R(0x11c07c) - 0xf9), (uint32_t)ry, 0xffffffffu, 5);
    if (e.blk) e.blk(e.ctx, 0xd096, rp, 18);
    rec[0] = R(0x11c064);
    e.H(0xd096, rp, (uint32_t)(half + 0xf9), (uint32_t)ry, 0xffffffffu, 5);
    if (e.blk) e.blk(e.ctx, 0xd096, rp, 18);
    const uint32_t now = g.now;
    if (now - R(0x123404) > 0x32) {
        uint32_t a = (R(0x11c064) + 1) & 0x80000007u;
        W(0x123404, now);
        if ((int32_t)a < 0) a = ((a - 1) | 0xfffffff8u) + 1;
        W(0x11c064, a);
    }
}
