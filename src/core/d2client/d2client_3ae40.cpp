// 1.13c D2Client+0x3AE40 -- key command: set the run flag at 0x11C3E8; a living player unit (type 0) in mode 2 that
// passes the three-word check (player, 0xA, 0) is started (callee with ESI = player, EDI = 3) and the byte 0x53 goes
// out through the one-byte send (AL). Without a player EAX is left as it came in.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3ae40(uint32_t eax, const cl::Env& e) {
    const uint32_t pl = clc::G(e, clc::kPlayer);
    clc::S(e, 0x11C3E8, 1);
    if (!pl) return eax;
    eax = clc::G(e, clc::kBusy);
    if (eax) return eax;
    if (e.R(pl + 0x10) == clc::kDeadMode || e.R(pl) != 0 || e.R(pl + 0x10) != 2) return 0;
    if (!e.H(0xc1c6, pl, 0xa, 0)) return 0;
    e.H(0x2a6c0, 0, 0, 0, 0, 0, 0, 0, 0, 0, pl);
    e.Edj(0x2a6c0, 3);
    return e.H(0x14a30, 0, 0, 0, 0, 0, 0x53);
}
