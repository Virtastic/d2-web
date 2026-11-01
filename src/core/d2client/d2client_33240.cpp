// Runs the two unit-stat helpers over the same unit, the pair callback used by the skill-description line builders.
#include "d2client_33240.h"
namespace d2client_33240 {
// The first helper takes (ecx = unit, eax = s2; stack edx, s0, s1, s3, s4, s5, s6, 0), then the second takes (unit, edx, s0, s1, s4); the result is the second's.
uint32_t sub_33240(Ops& o, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6) {
    const uint32_t w[8] = {edx, s0, s1, s3, s4, s5, s6, 0};
    o.c31220(ecx, s2, w);
    return o.c30E30(ecx, edx, s0, s1, s4);
}
}
