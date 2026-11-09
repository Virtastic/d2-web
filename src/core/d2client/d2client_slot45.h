// Shared body of the two weapon-swap body-slot clicks, 1.13c D2Client+0x96280 (k = 5) and D2Client+0x96690 (k = 4), written
// from the disassembly. Both read the two hand items i5 = 0xC262(p2, 5) then i4 = 0xC262(p2, 4); "pri" is the item for slot k
// (the one the function keeps in EDI), "sec" the other. ECX = p1, EDX = p2, one stack argument A (ret 4); the local result starts 0.
// gA = 0x11B834 (+0x30 = 0x11B864 mode, +0x3FC = 0x11BC30, +0x400 = 0x11BC34), gB = 0x10B2A4 (+0x18C = 0x10B430).
#pragma once
#include "cl_env.h"
namespace d2c_slot45 {
inline uint32_t run(uint32_t p1, uint32_t p2, uint32_t a, uint32_t gA, uint32_t gB, const cl::Env& e, uint32_t k) {
    const uint32_t esi = e.H(0xc202, p2);
    const uint32_t i5 = e.H(0xc262, p2, 5), i4 = e.H(0xc262, p2, 4);
    const uint32_t pri = k == 5 ? i5 : i4, sec = k == 5 ? i4 : i5;
    const auto busy = [&](uint32_t it) { return e.H(0x81cb0, 0, 0, 0, 0, 0, it) != 0; };
    const auto ok = [&]() { return e.H(0x923f0); };   // EAX = 0 (the preceding 0x81CB0 returned 0)
    const auto id = [&](uint32_t it) { return it ? e.R(it + 0xc) : 0xffffffffu; };
    const auto fin = [&](uint32_t it) {   // 0x81CE0(it), 0x81E40 (ESI = it, EDI = 0), 0x88A70 with EBX = 0x81E40's result
        e.H(0x81ce0, 0, 0, 0, 0, 0, it);
        const uint32_t b = e.H(0x81e40, 0, 0, 0, 0, 0, 0, 0, 0, 0, it);
        e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, b, 0);
    };
    uint32_t ret = 0;
    const uint32_t mode = e.R(gA + 0x30);
    if (mode == 6) {   // AL = 0x27 over 0x923F0's result in 96280 (EBP holds g) but over g itself in 96690 (EAX reloaded from the local)
        const uint32_t g = e.R(gA);
        if (g && pri && !busy(pri)) {
            const uint32_t r = ok();
            if (r) { e.H(0x14910, 0, 0, 0, 0, 0, ((k == 5 ? r : g) & ~0xffu) | 0x27, e.R(pri + 0xc), e.R(g + 0xc)); e.H(0x81ce0, 0, 0, 0, 0, 0, pri); ret = 1; }
        }
    } else if (mode == 8) {
        if (pri && !busy(pri)) {
            const uint32_t r = ok();
            if (r) { e.H(0x14940, 0, 0, 0, 0, 0, (r & ~0xffu) | 0x4c, e.R(pri + 0xc)); e.H(0x81ce0, 0, 0, 0, 0, 0, pri); ret = 1; }
        }
    }
    const uint32_t sel = e.H(0xc976, p1, a, esi, 0);
    const auto f95 = [&](uint32_t it) { e.H(0x95f80, p1, 1, e.R(gB), 0, 0, 0, it, e.R(gB + 0x18c), 0, 0); };
    const auto send = [&](uint32_t r, uint32_t ty, uint32_t c, uint32_t d) { e.H(0x14910, 0, 0, 0, 0, 0, (r & ~0xffu) | ty, c, d); };
    switch (sel) {
    case 0:
        if (!esi) return 1;
        if (e.H(0xc976, p1, a, esi, 1)) {
            const uint32_t t = e.H(0xc9a0, esi);
            if (t == 7 || t == (p1 ? e.R(p1 + 4) : 0xffffffffu)) { e.H(0x23200, p1, 0x14); return 1; }
        }
        e.H(0x23200, p1, 0x13);
        return 1;
    case 1:
    case 5: {
        if (busy(sel == 1 ? esi : pri)) return ret;
        const uint32_t r = ok();
        if (!r) return ret;
        send(r, sel == 1 ? 0x1a : 0x1d, id(esi), k);
        fin(esi);
        return 1;
    }
    case 2: {
        if (busy(esi) || busy(sec)) return ret;
        const uint32_t r = ok();
        if (!r) return ret;
        send(r, 0x1b, id(esi), k);
        e.H(0x81ce0, 0, 0, 0, 0, 0, sec);
        fin(esi);
        e.W(gA + 0x3fc, 1);
        return 1;
    }
    case 3:
    case 4: {
        const uint32_t it = sel == 3 ? pri : sec;
        if (sel == 3) {
            if (!e.H(0x8be10, 0, 0, 0, 0, 0, 3)) {
                if ((int32_t)e.R(gA + 0x400) < 0x13) f95(pri);
                return ret;
            }
        } else if (e.R(gA + 0x400) == 4) { f95(sec); return ret; }
        if (busy(it)) return ret;
        const uint32_t r = ok();
        if (!r) return ret;
        e.H(0x14a10, 0, 0, 0, 0, 0, (r & ~0xffu) | 0x1c, sel == 3 ? k : 9 - k);
        e.H(0x81ce0, 0, 0, 0, 0, 0, it);
        e.W(gA + 0x3fc, 1);
        return 1;
    }
    case 6: {
        if (busy(esi)) return ret;
        const uint32_t r = ok();
        if (!r || e.R(gA + 0x400) == 0xb) return ret;
        send(r, 0x21, id(esi), id(pri));
        fin(esi);
        return 1;
    }
    case 7: {
        if (busy(esi) || busy(i4) || busy(i5)) return ret;
        const uint32_t r = ok();
        if (!r) return ret;
        send(r, 0x1e, id(esi), k);
        e.H(0x81ce0, 0, 0, 0, 0, 0, esi);
        e.H(0x81ce0, 0, 0, 0, 0, 0, i4);
        e.H(0x81ce0, 0, 0, 0, 0, 0, i5);
        return 1;
    }
    default: return ret;
    }
}
}  // namespace d2c_slot45
