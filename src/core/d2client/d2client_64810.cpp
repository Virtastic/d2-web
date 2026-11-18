// 1.13c D2Client+0x64810 -- options item (contrast): default 100 unless the renderer mode is 4, load the saved value, scale it into the slot, save it back, apply it.
#include "cl_cmd.h"
namespace d2client_lerp {
uint64_t lerp63620(uint32_t a, uint32_t b, uint32_t n, uint32_t m);
uint64_t lerp636a0(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t n);
}
extern "C" uint32_t d2c_64810(uint32_t it, const cl::Env& e) {
    uint32_t loc = e.H(0xd10e) != 4 ? 0x64u : 0u;
    const uint32_t la = (uint32_t)(uintptr_t)&loc;
    e.H(0xbe8a, 0x6FB83588u, 0x6FB851BCu, 0, la);
    e.W(it + 0x124, (uint32_t)d2client_lerp::lerp636a0(e.R(la), 0x64, 0, e.R(it + 0x120)));
    const uint32_t v = (uint32_t)d2client_lerp::lerp63620(0x64, 0, e.R(it + 0x120), e.R(it + 0x124));
    e.H(0xbef6, 0x6FB83588u, 0x6FB851BCu, 0, v);
    e.H(0xd150, 0, 0, 0, 0, 0, 0, 0xB, e.R(it + 0x124));
    return e.H(0xd13e, 0);
}
