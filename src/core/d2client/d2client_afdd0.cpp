// 1.13c D2Client+0xAFDD0 (no arguments): stamps GetTickCount into 0xFAE1C, calls 0x3CB00 (EBX = the callback at 0xAFCA0) and 0xD22E, calls 0xD216 when the
// connection state 0x11C394 is 0, 1, 6 or 8, then tail-calls 0x109C0.
#include "cl_genv.h"
using namespace cl;
// 1.13c D2Client+0xAFDD0
extern "C" uint32_t d2c13_afdd0(const Env& e, const GEnv& g) {
    e.W(g.G(0xfae1c), e.H(0xcef5c));
    e.H(0x3cb00, 0, 0, 0, 0, 0, 0, 0, 0, g.M(0xafca0));
    e.H(0xd22e);
    uint32_t st = e.R(g.G(0x11c394));
    if (st == 0 || st == 1 || st == 6 || st == 8) e.H(0xd216);
    return e.H(0x109c0);   // tail jump: EAX is the callee's
}
