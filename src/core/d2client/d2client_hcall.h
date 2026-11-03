// Host-seam call helper for the d2client_<rva>.cpp ports: one D2cCall per callee, registers and stack words as the retail call site sets them.
#pragma once
#include <cstdint>
#include "client_host.h"
static inline uint32_t d2cCall(D2cHost h, uint32_t rva, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0,
                               uint32_t eax = 0, uint32_t ecx = 0, uint32_t edx = 0, uint32_t ebx = 0, uint32_t esi = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = eax; c.ecx = ecx; c.edx = edx; c.ebx = ebx; c.esi = esi; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3; return h(&c);
}
