// Unit query (type, id); when its value for 0x161 is below 6, pass it with the value for 0x162 to the common setter.
#include "cl_env.h"
// 1.13c D2Client+0x72040. ECX = type, EDX = id.
extern "C" void d2cl_72040(uint32_t type, uint32_t id, const cl::Env& e) {
    uint32_t u = e.H(0xc1c0, type, id);
    if (!u) return;
    uint32_t a = e.H(0xc4b4, u, 0x161, 0);
    if (a >= 6) return;
    uint32_t b = e.H(0xc4b4, u, 0x162, 0);
    e.H(0xc532, type, a, b);
}
