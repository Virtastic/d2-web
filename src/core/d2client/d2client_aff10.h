// D2Client+0xAFF10 env seam: builds the 0x17-dword request for D2Client+0xB30F0 (ECX = request, returns the unit; this caller ignores it).
#pragma once
#include <cstdint>
struct D2cAFF10Env {
    void (*fB30F0)(const uint32_t* req);
};
// ECX unused, EDX = unit, stack words s0..s4 (ret 0x14)
extern "C" void d2c_aff10(const D2cAFF10Env* e, uintptr_t unit, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
