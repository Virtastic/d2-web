// 1.13c D2Client+0x8C1E0: ESI = destination wide string. Fetches string 0xF9E then 0x4FD6
// (D2Lang #10003, thunk 0xD1EC, ECX = id) and appends the second, then the first (Unicode strcat import 0xCEABC).
#include "cl_env.h"
namespace { void ucat(uint32_t d, uint32_t s) {
    uint16_t* p = (uint16_t*)(uintptr_t)d; const uint16_t* q = (const uint16_t*)(uintptr_t)s;
    while (*p) ++p;
    while ((*p++ = *q++)) {}
} }
extern "C" void d2cl_8c1e0(uint32_t dst, const cl::Env& e) {
    const uint32_t a = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xf9e);
    const uint32_t b = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x4fd6);
    ucat(dst, b);
    ucat(dst, a);
}
