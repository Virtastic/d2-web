// 1.13c D2CMP+0x2520 (stdcall p): SEH frame; constructs the bucket at p (0x21C0) when p is non-null.
#include <cstdint>
extern "C" uint32_t d2cmp_21c0(uint8_t* m, uint32_t p);
// 1.13c D2CMP+0x2520
extern "C" uint32_t d2cmp_2520(uint8_t* m, uint32_t p) {
    if (p) d2cmp_21c0(m, p);
    return p;
}
