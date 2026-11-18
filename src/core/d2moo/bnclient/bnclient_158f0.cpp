// 1.13c BNClient+0x158F0 (random_key): fill out[0..length-2] with the nonzero low bytes of MSVC rand() seeded 0x150b,
// reseed the generator with the clock (time_seed), then out[length-1] = 0. The fill is the deterministic part of the
// routine; the reseed affects only later rand() calls. Stack word 0 is the length, EBX is the output.
#include <cstdint>

#include "../crt/msvc_rand.h"

extern "C" void bnclient_158f0(uint8_t* out, int32_t length, uint32_t time_seed) {
    if (length <= 0) return;  // retail writes out[-1] here; no caller passes 0
    d2::crt::msvc_srand(0x150bu);
    for (int32_t i = 0; i < length - 1;) {
        const uint8_t value = uint8_t(d2::crt::msvc_rand());
        if (value) out[i++] = value;
    }
    d2::crt::msvc_srand(time_seed);
    out[length - 1] = 0;
}
