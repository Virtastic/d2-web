// 1.13c D2CMP+0xC9D0: table select. stdcall(alt).
#include <cstdint>
// enabled = [6FE32CC8], active = [6FE32CCC], fallback = [6FE32CD0]
// 1.13c D2CMP+0xC9D0
extern "C" uint32_t d2cmp_c9d0(uint32_t alt, uint32_t enabled, uint32_t active, uint32_t fallback) {
    return (alt != 0 && enabled != 0) ? active : fallback;
}
