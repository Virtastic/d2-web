// 1.13c D2Client+0x95FE0, written from the disassembly. ECX = p1 (a unit), EDX = p2, one stack argument A (ret 4).
// esi = 0xC202(p2). By the mode word [0x11B864]: 6 and 8 send a packet about the item 0xC262(p2, A) (6: type 0x27 with [[0x11B834]+0xC] and
// the item id, 8: 0x4C), both only when the item and 0x81CB0(item) == 0 and 0x923F0() nonzero, then 0x81CE0(item) and return 1. Any other mode
// dispatches on 0xC976(p1, A, esi, 0) (0..5, above 5: 0): 0 reports 0x23200(p1, 0x13 or 0x14); 1 and 5 send type 0x1A / 0x1D (id, A & 0xFFFF),
// run 0x81CE0, 0x81E40, 0x88A70 and return 1 (1 also sets [0x11BC30]); 3 sends 0x1C (arg A) when 0x8BE10() and 0x923F0() are nonzero (sets
// [0x11BC30], returns 1), else with [0x11BC34] < 0x13 runs 0x95F80 on the item 0xC262(p2, A) and returns 0. Every other exit returns the local (0).
// The packet senders 0x14910/0x14940/0x14A10 take the type in AL, which keeps the upper bytes of 0x923F0's result: (r & ~0xFF) | type.
#include "cl_env.h"
// 1.13c D2Client+0x95FE0
extern "C" uint32_t d2c_95fe0(uint32_t p1, uint32_t p2, uint32_t a, uint32_t gA, uint32_t gB, const cl::Env& e) {
    const uint32_t esi0 = e.H(0xc202, p2);
    const uint32_t mode = e.R(gA + 0x30);   // 0x11B864; 0x11B834 = gA, 0x11BC30 = gA+0x3FC, 0x11BC34 = gA+0x400, 0x10B2A4 = gB, 0x10B430 = gB+0x18C
    const auto tail = [&](uint32_t item) {   // 0x81CE0, 0x81E40, 0x88A70
        e.H(0x81ce0, 0, 0, 0, 0, 0, item);
        const uint32_t b = e.H(0x81e40);
        e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, b, 0);
    };
    if (mode == 6) {
        const uint32_t edi = e.R(gA), esi = e.H(0xc262, p2, a);
        if (!edi || !esi || e.H(0x81cb0, 0, 0, 0, 0, 0, esi)) return 0;
        const uint32_t r = e.H(0x923f0, 0, 0, 0, 0, 0, 0);
        if (!r) return 0;
        e.H(0x14910, 0, 0, 0, 0, 0, (r & ~0xffu) | 0x27, e.R(esi + 0xc), e.R(edi + 0xc));
        e.H(0x81ce0, 0, 0, 0, 0, 0, esi);
        return 1;
    }
    if (mode == 8) {
        const uint32_t esi = e.H(0xc262, p2, a);
        if (!esi || e.H(0x81cb0, 0, 0, 0, 0, 0, esi)) return 0;
        const uint32_t r = e.H(0x923f0, 0, 0, 0, 0, 0, 0);
        if (!r) return 0;
        e.H(0x14940, 0, 0, 0, 0, 0, (r & ~0xffu) | 0x4c, e.R(esi + 0xc));
        e.H(0x81ce0, 0, 0, 0, 0, 0, esi);
        return 1;
    }
    const uint32_t sel = e.H(0xc976, p1, a, esi0, 0);
    if (sel > 5) return 0;
    switch (sel) {
    case 0: {
        if (!esi0) return 1;
        if (!e.H(0xc976, p1, a, esi0, 1)) { e.H(0x23200, p1, 0x13); return 1; }
        const uint32_t t = e.H(0xc9a0, esi0);
        const uint32_t ecx = p1 ? e.R(p1 + 4) : 0xffffffffu;
        e.H(0x23200, p1, (t == 7 || t == ecx) ? 0x14u : 0x13u);
        return 1;
    }
    case 1:
    case 5: {
        if (e.H(0x81cb0, 0, 0, 0, 0, 0, esi0)) return 0;
        const uint32_t r = e.H(0x923f0, 0, 0, 0, 0, 0, 0);
        if (!r) return 0;
        const uint32_t ecx = esi0 ? e.R(esi0 + 0xc) : 0xffffffffu;
        e.H(0x14910, 0, 0, 0, 0, 0, (r & ~0xffu) | (sel == 1 ? 0x1au : 0x1du), ecx, a & 0xffff);
        tail(esi0);
        if (sel == 1) e.W(gA + 0x3fc, 1);
        return 1;
    }
    case 3: {
        const uint32_t r3 = e.H(0x8be10, 0, 0, 0, 0, 0, 3);
        if (r3) {
            const uint32_t r = e.H(0x923f0, 0, 0, 0, 0, 0, r3);
            if (!r) return 0;
            e.H(0x14a10, 0, 0, 0, 0, 0, (r & ~0xffu) | 0x1c, a);
            e.W(gA + 0x3fc, 1);
            return 1;
        }
        if ((int32_t)e.R(gA + 0x400) >= 0x13) return 0;
        const uint32_t it = e.H(0xc262, p2, a);
        e.H(0x95f80, p1, 1, e.R(gB), 0, 0, 0, it, e.R(gB + 0x18c), 0, 0);
        return 0;
    }
    default: return 0;
    }
}
