// 1.13c D2Client+0x63580 -- options item (blended shadows): load the saved value into the item's value slot (+0x124), hand it to the renderer.
#include "cl_cmd.h"
extern "C" uint32_t d2c_63580(uint32_t it, const cl::Env& e) {
    e.H(0xbe8a, 0x6FB83588u, 0x6FB851E8u, 0, it + 0x124);
    return e.H(0xd144, e.R(it + 0x124));
}
