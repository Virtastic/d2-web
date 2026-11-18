// 1.13c D2Client+0x2E410 (713 B, stdcall ret 0x14: a1, dst, a3, item, a5): 0x2E900 plus stat 0x6F entries, whose C316 result is added to both A and B (before the percent scaling).
#include "d2client_tip.h"
extern "C" uint32_t d2c_port_2e410(const D2cTipEnv& t, uint32_t a1, uint32_t dst, uint32_t a3, uint32_t item, uint32_t a5) {
    return d2c_tip_body(t, a1, dst, a3, item, a5, true);
}
