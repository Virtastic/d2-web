// 1.13c D2Client+0x39C30 -- key command: walk the list at 0x113AA4 and clear every node's dword at +0x1E (next link at +0x22).
#include "cl_cmd.h"
extern "C" uint32_t d2c_39c30(uint32_t, const cl::Env& e) {
    for (uint32_t p = clc::G(e, 0x113AA4); p; p = e.R(p + 0x22)) e.W(p + 0x1e, 0);
    return 0;
}
