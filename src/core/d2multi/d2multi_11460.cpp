// 1.13c D2Multi+0x11460: stdcall (p1, p2) with ECX = this; runs 0x11290 with EAX = this, EDI = p1, ECX = p2; returns 1.
#include "d2multi_env.h"

void d2multi_11290(const D2mEnv& e, uint32_t eax, uint32_t ecx, uint32_t edi, uint32_t dat0a090);
uint32_t d2multi_11460(const D2mEnv& e, uint32_t self, uint32_t p1, uint32_t p2, uint32_t dat0a090) {
    d2multi_11290(e, self, p2, p1, dat0a090);
    return 1;
}
