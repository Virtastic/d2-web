// 1.13c D2Client+0x8C760: EDI = out flag, EBX = unit, stack p (ret 4). *out = 0; with the gate word
// D2Client+0x119854 clear returns 0. Two D2Common flag checks (thunk 0xC1A8), then state tests (thunk 0xC1CC, stdcall 2) chosen by
// the skill id 0x216D0 returns for p's +0xC word (ECX, -1 without p); a hit sets *out = 1. Returns 1 only when a hit happened and
// the D2Common check behind thunk 0xC898 (stdcall 6: unit, p, 0, 0, 0, 0) accepts.
#include "cl_env.h"
extern "C" uint32_t d2cl_8c760(uint32_t out, uint32_t unit, uint32_t p, const cl::Env& e) {
    e.W(out, 0);
    if (!e.R(0x6FAB0000u + 0x119854u)) return 0;
    const uint32_t F = 0xB2000000u | 0x8476Cu;
    if (!e.H(0xc1a8, unit, 0x10, 0x301b, F)) return 0;
    if (e.H(0xc1a8, unit, 0x100, 0x301e, F)) return 0;
    uint32_t on = 0;
    auto st = [&](uint32_t s) { return e.H(0xc1cc, unit, s) != 0; };
    auto set = [&]() { on = 1; e.W(out, 1); };
    if (st(3) || st(0x25)) set();
    const uint32_t id = e.H(0x216d0, 0, 0, 0, 0, 0, 0, p ? e.R(p + 0xc) : 0xffffffffu);
    bool hit = false;
    switch (id) {
    case 0x167: if (st(2)) set(); hit = st(0x1e) && !e.H(0xc87a, unit); break;
    case 0x10f: hit = st(0x1b); break;
    case 0x152: hit = st(0x21) || st(0x22); break;
    case 0x230: if (st(0x1c) && !e.H(0xc87a, unit)) set(); hit = st(0x47); break;
    case 0x231: if (st(0x1e)) set(); hit = st(0x47); break;
    default: break;
    }
    if (hit) set();
    else if (!on) return 0;
    if (!e.H(0xc898, unit, p, 0, 0, 0)) on = 0;
    return on;
}
