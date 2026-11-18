// Unit state gate: the same query as the hero state helper, false for states 2 and 7.
#include "cl_env.h"
// 1.13c D2Client+0x81F60. ESI = unit, EAX = the lookup's second argument.
extern "C" uint32_t d2cl_81f60(uint32_t unit, uint32_t eax, const cl::Env& e) {
    uint32_t r = e.H(0xcb74, unit, eax);
    if (!r) r = (unit == e.R(0x6FAB0000u + 0x11bbfc) && e.R(0x6FAB0000u + 0x1197f8) < e.R(0x6FAB0000u + 0x11985c)) ? 8u : 0u;
    return (r != 2 && r != 7) ? 1u : 0u;
}
