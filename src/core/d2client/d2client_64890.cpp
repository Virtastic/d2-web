// 1.13c D2Client+0x64890 -- options item (gamma): default 0x9B, load the saved value, scale it into the slot; with the renderer able, save the re-scaled value and apply it.
#include "cl_cmd.h"
namespace d2client_lerp {
uint64_t lerp63620(uint32_t a, uint32_t b, uint32_t n, uint32_t m);
uint64_t lerp636a0(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t n);
}
extern "C" uint32_t d2c_64890(uint32_t it, const cl::Env& e) {
    uint32_t loc = 0x9B;
    const uint32_t la = (uint32_t)(uintptr_t)&loc;
    e.H(0xbe8a, 0x6FB83588u, 0x6FB851C8u, 0, la);
    e.W(it + 0x124, (uint32_t)d2client_lerp::lerp636a0(e.R(la), 0xFF, 0x37, e.R(it + 0x120)));
    const uint32_t able = e.H(0xd15c);
    if (!able) return able;
    const uint32_t v = (uint32_t)d2client_lerp::lerp63620(0xFF, 0x37, e.R(it + 0x120), e.R(it + 0x124));
    e.H(0xbef6, 0x6FB83588u, 0x6FB851C8u, 0, v);
    return e.H(0xd13e, v);
}
