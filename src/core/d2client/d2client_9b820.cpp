// 1.13c D2Client+0x9B820 -- stdcall(msg): a mouse press in the inventory-style panels (UI state [0x11BC34] 0xC or 0xD; any other state
// returns that value). x = word msg+0xC, y = word +0xE, stored to [0x10B2A4] / [0x10B430]. Hit tests in order, each ending in the
// Storm 511 message hand-off (FUN 0x88A70 with EBX = 4 and four zero words first where the arm sets a state flag):
//   0x8BD90(ECX x, EDX y) -> [0x11BEDC] = 1                      (tested when [0x11C474] == 0 or 0x26320(msg) == 0)
//   0x8B6B0(y)            -> [0x11BE80] = 1
//   0x8CD00(ESI x, EDI y) -> [0x11BE88] = [0x11BE84] = [0x11BEEC] = 1
// then 0x26320(msg) == 1 clears 0x11BE80/64/EC/84/6C/88 and returns 0; else 0x8C580 / 0x8C4C0(EAX x, ECX y) -> FUN 0x98DC0(msg), Storm
// 511, [0x11BE6C] = [0x11BE88] = 0; 0x8D3D0(EAX y, EDI x) -> [0x11BE7C] = 1, clears 6C/64/EC/84, 0x88A70, Storm 511, clears 6C/88;
// 0x8B6B0(y) -> [0x11BE80] = 1, clears 6C/64/EC/84, 0x88A70, Storm 511, [0x11BE88] = 0; 0x8C470(EAX y, ECX x) -> clears 80/6C/64/EC/84/88,
// Storm 511; else FUN 0x8F880(ESI msg, EDI x, y), clears 6C/88, returns 0. A Storm 511 arm returns its value.
#include "cl_cmd.h"
extern "C" uint32_t d2c_9b820(uint32_t msg, const cl::Env& e) {
    using namespace clc;
    uint32_t mode = G(e, 0x11BC34);
    if (mode != 0xc && mode != 0xd) return mode;
    uint32_t w = e.R(msg + 0xc), x = w & 0xffff, y = w >> 16;
    S(e, 0x10B2A4, x); S(e, 0x10B430, y);
    auto fin = [&]() { e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 4); return e.H(0xbe84, msg); };
    auto mark = [&]() { e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0); };
    bool skip = false;
    if (G(e, 0x11C474) != 0) skip = e.H(0x26320, 0, 0, 0, 0, 0, msg) != 0;
    if (!skip && e.H(0x8bd90, 0, 0, 0, 0, 0, 0, x, y)) { S(e, 0x11BEDC, 1); mark(); return fin(); }
    if (e.H(0x8b6b0, 0, 0, 0, 0, 0, y)) { S(e, 0x11BE80, 1); mark(); return fin(); }
    if (e.H(0x8cd00, 0, 0, 0, 0, 0, 0, 0, 0, 0, x)) {
        e.Ej(0x8cd00, y);
        S(e, 0x11BE88, 1); S(e, 0x11BE84, 1); S(e, 0x11BEEC, 1); mark(); return fin();
    }
    e.Ej(0x8cd00, y);
    if (e.H(0x26320, 0, 0, 0, 0, 0, msg) == 1) {
        S(e, 0x11BE80, 0); S(e, 0x11BE64, 0); S(e, 0x11BEEC, 0); S(e, 0x11BE84, 0); S(e, 0x11BE6C, 0); S(e, 0x11BE88, 0);
        return 0;
    }
    if (e.H(0x8c580, 0, 0, 0, 0, 0, x, y) || e.H(0x8c4c0, 0, 0, 0, 0, 0, x, y)) {
        e.H(0x98dc0, msg); mark();
        uint32_t r = e.H(0xbe84, msg);
        S(e, 0x11BE6C, 0); S(e, 0x11BE88, 0);
        return r;
    }
    if (e.H(0x8d3d0, 0, 0, 0, 0, 0, y)) {
        e.Ej(0x8d3d0, x);
        S(e, 0x11BE7C, 1); S(e, 0x11BE6C, 0); S(e, 0x11BE64, 0); S(e, 0x11BEEC, 0); S(e, 0x11BE84, 0); mark();
        uint32_t r = fin();
        S(e, 0x11BE6C, 0); S(e, 0x11BE88, 0);
        return r;
    }
    e.Ej(0x8d3d0, x);
    if (e.H(0x8b6b0, 0, 0, 0, 0, 0, y)) {
        S(e, 0x11BE80, 1); S(e, 0x11BE6C, 0); S(e, 0x11BE64, 0); S(e, 0x11BEEC, 0); S(e, 0x11BE84, 0); mark();
        uint32_t r = fin();
        S(e, 0x11BE88, 0);
        return r;
    }
    if (e.H(0x8c470, 0, 0, 0, 0, 0, y, x)) {
        S(e, 0x11BE80, 0); S(e, 0x11BE6C, 0); S(e, 0x11BE64, 0); S(e, 0x11BEEC, 0); S(e, 0x11BE84, 0); S(e, 0x11BE88, 0); mark();
        return e.H(0xbe84, msg);
    }
    e.H(0x8f880, y, 0, 0, 0, 0, 0, 0, 0, 0, msg);
    e.Ej(0x8f880, x);
    S(e, 0x11BE6C, 0); S(e, 0x11BE88, 0);
    return 0;
}
