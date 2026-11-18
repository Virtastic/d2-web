// 1.13c D2Client+0x6A550, written from the disassembly (the decompile's &param_3 / stack0xffffffec operands are the arg slots). EAX = unit,
// stdcall 4 (ox, oy, px, py), returns 0 or 1. The preamble is 0x6A3D0's (key, 0xC2AA mode, fatal 0x3AC, 0x1AF0 node or 0x6A2F0, then
// 0xC65E / 0xC658(unit, &type, &class, &id, "Gfx.cpp", 0xD90)); box f = [[node(id)+8]+0x14] (missing: 0). Hit when
// f+8 + ox - 0x10 <= px < f+0xC + ox + 0x10 and f+0x10 + oy - 0x10 <= py < f+0x14 + oy + 0x10 (signed).
#include "cl_env.h"
// 1.13c D2Client+0x6A550
extern "C" uint32_t d2c_6a550(uint32_t unit, uint32_t ox, uint32_t oy, uint32_t px, uint32_t py, const cl::Env& e) {
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
    e.Xs(0xd90);
    e.H(0xc658, unit, P(&type), P(&cls), P(&id), 0xB20D4FA4u);
    const uint32_t pb = e.R(unit + 0x54);
    if (!pb) return 0;
    n = e.R(pb + 0x28);
    if (!n) return 0;
    while (e.R(n) != id) { n = e.R(n + 0x10); if (!n) return 0; }
    const uint32_t f = e.R(e.R(n + 8) + 0x14);
    if (!f) return 0;
    if ((int32_t)px < (int32_t)(e.R(f + 8) + ox - 0x10)) return 0;
    if ((int32_t)px >= (int32_t)(e.R(f + 0xc) + ox + 0x10)) return 0;
    if ((int32_t)py < (int32_t)(e.R(f + 0x10) + oy - 0x10)) return 0;
    if ((int32_t)py >= (int32_t)(e.R(f + 0x14) + oy + 0x10)) return 0;
    return 1;
}
