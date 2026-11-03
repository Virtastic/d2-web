// 1.13c D2Multi+0x11270: stdcall (p1, p2); calls 0x10B70 with EBX = p1; returns 1.
#include "d2multi_env.h"

uint32_t d2multi_11270(const D2mEnv& e, uint32_t p1, uint32_t) {
    d2m_call(e, 0x10b70, 0, 0, 0, p1);
    return 1;
}
