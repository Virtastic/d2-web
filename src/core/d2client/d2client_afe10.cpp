// 1.13c D2Client+0xAFE10 (no arguments): clears 0x11C39C, then by the connection state 0x11C394: 0 and 1 call 0xD234 / 0xD222 with the state's constants, 4 calls 0xD222
// with the string at 0xF4428, 6 and 8 call 0xD234 / 0xD240 / 0xD23A / 0xD222 (string 0xF4320), any other state calls 0xAFD10 (EAX = string 0xF4320, ECX = 0) and skips to the
// stamp. 0, 1, 6 and 8 then run 0x3CB00 (EBX = 0xAFC40; for 0 and 1 after 0xD222, for 6 and 8 after 0xD222 too), 0xD23A(0) (0 and 1 only), 0xD03C(0x122360, 0x11C3B4), set 0xFAE18 and
// call 0xD030. Every path ends by stamping GetTickCount into 0xFAE24, clearing 0xFAE20, calling 0x3CB00 (EBX = 0xAFD30) and tail-calling 0x109C0.
#include "cl_genv.h"
using namespace cl;
// 1.13c D2Client+0xAFE10
extern "C" uint32_t d2c13_afe10(const Env& e, const GEnv& g) {
    auto cb = [&](uint32_t rva) { e.H(0x3cb00, 0, 0, 0, 0, 0, 0, 0, 0, g.M(rva)); };
    e.W(g.G(0x11c39c), 0);
    uint32_t st = e.R(g.G(0x11c394));
    bool tail = true;   // run the 0xD03C / 0xFAE18 / 0xD030 block
    if (st == 0 || st == 1) {
        e.H(0xd234, st == 0 ? 1 : 2, 1);
        e.H(0xd222, st == 0 ? 1 : 2, 0);
        cb(0xafc40);
        e.H(0xd23a, 0);
    } else if (st == 4) {
        e.H(0xd222, 0, g.M(0xf4428));
        cb(0xafc40);
        tail = false;
    } else if (st != 6 && st != 8) {
        e.H(0xafd10, 0, 0, 0, 0, 0, g.M(0xf4320), 0);
        tail = false;
    } else {
        e.H(0xd234, 0, 1);
        e.H(0xd240, 8);
        e.H(0xd23a, 0);
        e.H(0xd222, 0, g.M(0xf4320));
        cb(0xafc40);
    }
    if (tail) {
        e.H(0xd03c, g.M(0x122360), g.M(0x11c3b4));
        e.W(g.G(0xfae18), 1);
        e.H(0xd030);
    }
    e.W(g.G(0xfae24), e.H(0xcef5c));
    e.W(g.G(0xfae20), 0);
    cb(0xafd30);
    return e.H(0x109c0);   // tail jump: EAX is the callee's
}
