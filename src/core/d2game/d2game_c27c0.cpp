// 1.13c D2Game+0xC27C0 -- ECX = record A, EDX = record B. When B's +8 pointer is set and the 16-bit id at +4 of its target is 0x179, a flag byte of 1 behind A's +0x18 pointer is cleared together with A's +0xA8 field. EAX returns the last pointer read (B's +8, or A's +0x18).
#include <cstdint>
uint32_t d2game_c27c0(uint32_t* a, const uint32_t* b) {
    const uint32_t t = b[2];
    if (!t || ((const uint16_t*)(uintptr_t)t)[2] != 0x179) return t;
    uint8_t* flag = (uint8_t*)(uintptr_t)a[6];
    if (*flag == 1) { *flag = 0; a[0x2A] = 0; }
    return a[6];
}
