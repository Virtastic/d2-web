// MSVC CRT rand/srand: the linear congruential generator on one per-thread holdrand.
// 1.13c BNClient+0x15F2 (_srand) and +0x15FF (_rand); the same recurrence as the d2sound shim.
#pragma once
#include <cstdint>

namespace d2::crt {
void msvc_srand(uint32_t seed);
int msvc_rand();
}  // namespace d2::crt
