// 1.13c D2Client+0x64180 -- options item (light quality): load the saved value, 0 -> renderer 1 and flag 0, 1 -> renderer 0 and flag 0, 2 -> renderer 0 and flag 1.
#include "cl_cmd.h"
extern "C" uint32_t d2c_64180(uint32_t it, const cl::Env& e) {
    uint32_t r = e.H(0xbe8a, 0x6FB83588u, 0x6FB851F8u, 0, it + 0x124);
    switch (e.R(it + 0x124)) {
        case 0: r = e.H(0xd14a, 1); clc::S(e, 0xDD854, 0); break;
        case 1: r = e.H(0xd14a, 0); clc::S(e, 0xDD854, 0); break;
        case 2: r = e.H(0xd14a, 0); clc::S(e, 0xDD854, 1); break;
        default: break;
    }
    return r;
}
