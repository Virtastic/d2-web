// Unit state query: the common lookup, else 8 for the hero while one counter is below another.
#include "cl_env.h"
// 1.13c D2Client+0x72080. ESI = unit, EAX = the lookup's second argument.
extern "C" uint32_t d2cl_72080(uint32_t unit, uint32_t eax, const cl::Env& e) {
    uint32_t r = e.H(0xcb74, unit, eax);
    if (r) return r;
    if (unit == e.R(0x6FAB0000u + 0x11bbfc) && e.R(0x6FAB0000u + 0x1197f8) < e.R(0x6FAB0000u + 0x11985c)) return 8;
    return 0;
}
