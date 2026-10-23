// 1.13c Fog+0x24479: MSVC CRT x87 math-error epilogue (fscale of ST0 by a denormal/inf
// bias, then dispatch to the matherr handler Fog+0x24330 for domain code 0x1d, else
// __startOneArgErrorHandling). The handlers raise only the CRT errno/matherr side effect, which
// wasm never observes (no x87, no float in the port); the only live contract is the verdict.
#include <cstdint>

// Returns 1 if the error handler would be entered: result exponent zero or all-ones, or the
// x87 status word has the precision-exception bit and control word is not 0x27f/bit5.
extern "C" int fog_sub_24479(uint64_t st0_bits, uint16_t ctrl, uint16_t status) {  // 1.13c Fog+0x24479
    const uint64_t e = st0_bits & 0x7ff0000000000000ull;
    if (e == 0 || e == 0x7ff0000000000000ull) return 1;
    if (ctrl == 0x27f || (ctrl & 0x20) || !(status & 0x20)) return 0;
    return 1;
}
