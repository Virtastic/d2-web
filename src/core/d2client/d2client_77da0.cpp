// Trade screen: clear every "marked" slot (value 1) of the per-slot mark table, up to the hero's slot count.
#include "cl_env.h"
// 1.13c D2Client+0x77DA0. The count is fetched again on every pass.
extern "C" void d2cl_77da0(const cl::Env& e) {
    uint32_t self = e.R(0x6FAB0000u + 0x11bbfc);
    if (!self) return;
    for (int32_t i = 0; i < (int32_t)e.H(0xcb38, e.R(self + 4)); ++i) {
        uint32_t a = 0x6FAB0000u + 0x113d38 + 4u * (uint32_t)i;
        if (e.R(a) == 1) e.W(a, 0);
    }
}
