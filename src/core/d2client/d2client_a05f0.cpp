// 1.13c D2Client+0xA05F0: eax = pct, esi = struct; scales the fixed-point dword at +0x44 by (100-pct)%.
// Returns the leftover EAX (the signed-divide magic product).
#include <cstdint>
extern "C" uint32_t d2c_a05f0(uint32_t pct, uint8_t* st) {
    int32_t* v = (int32_t*)(st + 0x44);
    int32_t t = (100 - (int32_t)pct) * (*v >> 8);
    *v = (t / 100) * 0x100;
    return (uint32_t)t * 0x51eb851fu;
}
