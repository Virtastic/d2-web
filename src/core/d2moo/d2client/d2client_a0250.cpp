// 1.13c D2Client+0xa0250 -- game-state packet dispatch on ESI = packet (no stack arguments), by byte +5 through the 8-entry table at
// D2Client+0xA0300 (the code ends at 0xA02FD; 0xA0320 is the next function, not part of this one):
//  0: dword +1 into D2Client+0x10B21C, 0 into +0x10B210, tail jump to 0x9FF70 (ESI = packet)
//  1: tail jump to 0x9F9E0
//  4: +0x10B214 = 2; when +0x10B224 is set, 0xA3D20 and +0x10B224 = 0
//  5: cursor clear 0xC7D8(player inventory (+0x60), 0), 0x88A70(0, 0, 0, 0) with EBX = 5 when byte +6 is nonzero, else 3, then as 1
//  6, 7: 0x9F9E0 then a tail jump to 0x48150
//  2, 3 and past 7: fatal (D2Client line 0x354)
#include "cl_env.h"
// g: where retail's D2Client+0x10B210 block (0x18 bytes) lives
extern "C" void d2c_a0250(uint32_t pkt, uint32_t g, const cl::Env& e) {
    switch (e.R8(pkt + 5)) {
    case 0:
        e.W(g + 0xc, e.R(pkt + 1));                            // 0x10B21C
        e.W(g, 0);                                             // 0x10B210
        e.H(0x9ff70, 0, 0, 0, 0, 0, 0, 0, 0, 0, pkt);
        return;
    case 4:
        e.W(g + 4, 2);                                         // 0x10B214
        if (e.R(g + 0x14)) { e.H(0xa3d20); e.W(g + 0x14, 0); }  // 0x10B224
        return;
    case 5:
        e.H(0xc7d8, e.R(e.self + 0x60), 0);
        e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, e.R8(pkt + 6) ? 5u : 3u);
        [[fallthrough]];
    case 1:
        e.H(0x9f9e0);
        return;
    case 6: case 7:
        e.H(0x9f9e0);
        e.H(0x48150);
        return;
    default:
        return;                                                // fatal 0x354 in retail
    }
}
