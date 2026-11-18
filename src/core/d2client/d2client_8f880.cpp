// 1.13c D2Client+0x8F880 -- a click inside the play area (ESI = message, EDI = x, [esp+4] = y): only in game state 2 (0xD0AE) and
// when 0x26320(EAX = msg) does not take it, y <= H - 0x30, and not on the left half (x < W / 2) while panel mode [0x11C414] is 1 or
// (mode 3 and [0x11BC34] clear). Mode [0x11B864] 6 first calls 0xBEB90. Then msg+0x18 = 1, [0x11BC28] = [0x11BC2C] = 0 and the
// message goes on to Storm 511. g = pinned globals (+0 W [0xDBC48], +4 H [0xDBC4C], +8 [0x11C414], +0xC [0x11B864],
// +0x10 [0x11BC34], +0x14 [0x11BC28], +0x18 [0x11BC2C]).
#include "cl_env.h"
extern "C" void d2c_8f880(uint32_t msg, int32_t x, int32_t y, const cl::Env& e, uint32_t g) {
    if (e.H(0xd0ae) != 2) return;
    if (e.H(0x26320, 0, 0, 0, 0, 0, msg)) return;
    if (y > (int32_t)e.R(g + 4) - 0x30) return;
    int32_t half = (int32_t)e.R(g) / 2;
    if (e.R(g + 8) == 1 && x < half) return;
    if (e.R(g + 0xc) == 6) e.H(0xbeb90);
    if (e.R(g + 8) == 3 && e.R(g + 0x10) == 0 && x < (int32_t)e.R(g) / 2) return;
    e.W(msg + 0x18, 1);
    e.W(g + 0x14, 0); e.W(g + 0x18, 0);
    e.H(0xbe84, msg);
}
