// 1.13c D2Client+0x3ADF0 -- key command: clear the run flag at 0x11C3E8; a living player unit (type 0) in mode 3, with
// the flag at 0x11C3EC clear, is stopped (callee with ESI = player, EDI = 2) and the one-byte message {0x54} is sent.
// Without a player EAX is left as it came in.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3adf0(uint32_t eax, const cl::Env& e) {
    const uint32_t pl = clc::G(e, clc::kPlayer);
    clc::S(e, 0x11C3E8, 0);
    if (!pl) return eax;
    eax = clc::G(e, 0x11C3EC);
    if (eax) return eax;
    if (e.R(pl) != 0 || e.R(pl + 0x10) != 3) return 0;
    e.H(0x2a6c0, 0, 0, 0, 0, 0, 0, 0, 0, 0, pl);
    e.Edj(0x2a6c0, 2);
    const uint8_t b[4] = {0x54, 0, 0, 0};
    return e.H(0x143e0, (uint32_t)(uintptr_t)b, 0, 0, 0, 0, 0, 0, 0, 1, 0);
}
