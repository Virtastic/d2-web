// 1.13c D2Client+0xAEDF0 stat packets 0x19..0x1F, written from the disassembly: ECX = packet, the hero is the module global at +0x11BBFC.
// A null hero is fatal (text 0x9AA, then exit); nothing is written. Otherwise stat and value come from the opcode and go to the stat setter
// (0xC40C: unit, stat, value, layer 0) and the stat-change event (0xAEB10: EAX = stat, ECX = unit, stack value):
//   0x19 stat 14 = read(14) + byte; 0x1A stat 13 = read(13) + byte; 0x1B stat 13 = read(13) + word; 0x1C stat 13 = dword;
//   0x1D / 0x1E / 0x1F stat = byte at +1, value = byte / word / dword at +2; any other opcode: stat -1, value 0 (the setter and the event still run).
#include "cl_genv.h"
using namespace cl;
extern "C" void d2c13_aedf0(uint32_t pk, const Env& e, const GEnv& g) {
    const uint32_t hero = e.R(g.G(0x11bbfc));
    if (!hero) { Fatal(e, g, 0x9aa); return; }
    uint32_t stat = 0xffffffffu, value = 0;
    switch (B(pk)) {
    case 0x19: stat = 14; value = e.H(0xc1c6, hero, 14, 0) + B(pk + 1); break;
    case 0x1a: stat = 13; value = e.H(0xc1c6, hero, 13, 0) + B(pk + 1); break;
    case 0x1b: stat = 13; value = e.H(0xc1c6, hero, 13, 0) + W16(pk + 1); break;
    case 0x1c: stat = 13; value = e.R(pk + 1); break;
    case 0x1d: stat = B(pk + 1); value = B(pk + 2); break;
    case 0x1e: stat = B(pk + 1); value = W16(pk + 2); break;
    case 0x1f: stat = B(pk + 1); value = e.R(pk + 2); break;
    default: break;
    }
    e.H(0xc40c, hero, stat, value, 0);
    e.H(0xaeb10, value, 0, 0, 0, 0, stat, hero);
}
