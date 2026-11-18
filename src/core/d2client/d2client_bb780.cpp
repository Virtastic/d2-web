// 1.13c D2Client+0xBB780 (363 B, written from the disassembly): ECX = client missile unit, plain ret. Row = Missiles row of unit[+4]
// (range checked against *(dt+0xB6C), stride 0x1A4); out of range or null unit calls 0xB1720(). room = 0xA60D0(EAX = unit); 0 calls 0xB17C0(ECX = unit).
// Word row+0x1E > 0: v = D2Common#11018(unit); if v <= 0 { 0xB82B0(EAX = unit; room, word 0x1E, row+0x60); v = row+0x58 }; #10637(unit, v-1).
// Word row+0x20 > 0: v = #10818(unit); if v <= 0 { a zeroed 23-dword block {0x102, room, unit, 0, word 0x20, ..., [7] = [8] = 0x1330(2d+1) - d,
// [11] = #10240(unit), [12] = 0x1330(5) + 1 (EAX = n, ECX = unit+0x20), d = row+0x60; both zero: [7] = 0x1330(d-1) + 1 }; q = 0xB30F0(ECX = block);
// q != 0: #10637(q, 0x1330(5)); v = row+0x5C (the ROW, not the room) }; #10019(unit, v-1). Then 0xB17C0(ECX = unit).
#include "cl_env.h"
extern "C" void d2c_bb780(uint32_t u, const cl::Env& e) {
    const uint32_t row = u ? e.Row((int32_t)e.R(u + 4)) : 0;
    if (!row) { e.H(0xb1720); return; }
    const uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!p) { e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u); return; }
    const uint32_t d = e.R(row + 0x60);
    if (e.R16s(row + 0x1e) > 0) {
        int32_t v = (int32_t)e.H(0xc322, u);
        if (v <= 0) {
            e.H(0xb82b0, p, (uint32_t)e.R16s(row + 0x1e), d, 0, 0, u);
            v = (int32_t)e.R(row + 0x58);
        }
        e.H(0xc448, u, (uint32_t)(v - 1));
    }
    if (e.R16s(row + 0x20) > 0) {
        int32_t v = (int32_t)e.H(0xc4ba, u);
        if (v <= 0) {
            uint32_t q[23] = {};
            q[0] = 0x102; q[1] = p; q[2] = u; q[4] = (uint32_t)e.R16s(row + 0x20);
            q[11] = e.H(0xc388, u);
            const uint32_t us = u + 0x20;
            q[12] = e.H(0x1330, 0, 0, 0, 0, 0, 5, us) + 1;
            q[7] = e.H(0x1330, 0, 0, 0, 0, 0, d + d + 1, us) - d;
            q[8] = e.H(0x1330, 0, 0, 0, 0, 0, d + d + 1, us) - d;
            if (!q[7] && !q[8]) q[7] = e.H(0x1330, 0, 0, 0, 0, 0, d - 1, us) + 1;
            const uint32_t n = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
            if (n) e.H(0xc448, n, e.H(0x1330, 0, 0, 0, 0, 0, 5, us));
            v = (int32_t)e.R(row + 0x5c);
        }
        e.H(0xc394, u, (uint32_t)(v - 1));
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
