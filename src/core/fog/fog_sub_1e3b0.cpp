// 1.13c Fog+0x1E3B0 ordinal 10077 fog_sub_1e3b0: stores two words at +0x168/+0x16C of the struct; null is a no-op.
#include <cstdint>

extern "C"
void fog_sub_1e3b0(uint8_t *p, uint32_t a, uint32_t b) {
    if (p) {
        *(uint32_t *)(p + 0x168) = a;
        *(uint32_t *)(p + 0x16c) = b;
    }
}
