// 1.13c D2Client+0x491E0 -- speech-menu entry: calls the one-word routine with stack word 1 and EAX = the dword at
// +0xD of the current menu record (pointer at 0x119675). Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_491e0(const cl::Env& e) {
    const uint32_t rec = clt::G(e, 0x119675);
    e.H(0x48590, 1, 0, 0, 0, 0, e.R(rec + 0xd));
    return 1;
}
