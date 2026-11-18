// 1.13c D2Client+0x39DF0 -- key command: flip the config flag at 0xF16B8 and store it under its key name.
#include "cl_cmd.h"
extern "C" uint32_t d2c_39df0(uint32_t, const cl::Env& e) {
    uint32_t v;
    const uint32_t r = clc::cfgFlip(e, 0xF16B8, 0x6FB853B4u, v);
    clc::S(e, 0xF16B8, v);
    return r;
}
