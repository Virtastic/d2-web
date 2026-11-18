// 1.13c D2Client+0x6A3D0, written from the disassembly (the decompile's local_4 operands are the arg slots). EAX = unit, stdcall 3
// (dx, dy, centre), returns 0 or 1. key = +0x60 of 0xA60D0(EAX=unit) when bit 3 of +0xC8 is set and 0xCBDA(unit, 0x25) is nonzero (null: unit
// +0x60). mode = 0xC2AA(unit, key, &L, -1, 1). [unit+0x54] null is fatal 0x3AC. No node {0x1AF0(ESI=unit), mode} on [[unit+0x54]+0x28]
// (next +0x10): 0x6A2F0(EAX=-1, ECX=mode, EDX=unit). The id slot is preset by 0xC65E(unit) and rewritten by 0xC658(unit, &type, &class,
// &id, "Gfx.cpp", 0xDC2); the node of that id gives box f = [[node+8]+0x14] (missing: 0). x = f+8 + dx, less (f+0x14 - f+0x10)/2 (toward
// zero) when centre is nonzero: on screen when x < G8bc48-1, f+0xC + dx >= 0, f+0x14 + dy >= 0 and f+0x10 + dy < G8bc4c-1 (all signed).
#include "cl_env.h"
// 1.13c D2Client+0x6A3D0
extern "C" uint32_t d2c_6a3d0(uint32_t unit, uint32_t dx, uint32_t dy, uint32_t centre, uint32_t g, const cl::Env& e) {
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    uint32_t key;
    if ((e.R(unit + 0xc8) >> 3 & 1) && e.H(0xcbda, unit, 0x25)) {
        const uint32_t q = e.H(0xa60d0, 0, 0, 0, 0, 0, unit);
        key = q ? e.R(q + 0x60) : e.R(unit + 0x60);
    } else {
        key = e.R(unit + 0x60);
    }
    uint32_t l4 = 0;
    const uint32_t mode = e.H(0xc2aa, unit, key, P(&l4), 0xffffffffu, 1);
    const uint32_t pa = e.R(unit + 0x54);
    if (!pa) { e.H(0xbf80, 0xB20D359Fu, e.H(0xbf92, 0x3ac)); e.H(0x3664, 0xffffffffu); return 0; }
    const uint32_t k = e.H(0x1af0, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit);
    uint32_t n = e.R(pa + 0x28);
    while (n && !(e.R(n) == k && e.R(n + 4) == mode)) n = e.R(n + 0x10);
    if (!n) e.H(0x6a2f0, 0, 0, 0, 0, 0, 0xffffffffu, mode, unit);
    uint32_t type = e.R(unit), cls = e.R(unit + 4), id = e.H(0xc65e, unit);
    e.Xs(0xdc2);
    e.H(0xc658, unit, P(&type), P(&cls), P(&id), 0xB20D4FA4u);
    const uint32_t pb = e.R(unit + 0x54);
    if (!pb) return 0;
    n = e.R(pb + 0x28);
    if (!n) return 0;
    while (e.R(n) != id) { n = e.R(n + 0x10); if (!n) return 0; }
    const uint32_t f = e.R(e.R(n + 8) + 0x14);
    if (!f) return 0;
    const int32_t W = (int32_t)(e.R(g) - 1), Hh = (int32_t)(e.R(g + 4) - 1);
    uint32_t x = e.R(f + 8) + dx;
    if (centre) x -= (uint32_t)((int32_t)(e.R(f + 0x14) - e.R(f + 0x10)) / 2);
    if ((int32_t)x >= W) return 0;
    if ((int32_t)(e.R(f + 0xc) + dx) < 0) return 0;
    if ((int32_t)(e.R(f + 0x14) + dy) < 0) return 0;
    return (int32_t)(e.R(f + 0x10) + dy) < Hh ? 1u : 0u;
}
