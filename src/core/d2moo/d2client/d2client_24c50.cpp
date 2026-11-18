// 1.13c D2Client+0x24C50 -- jump-table handler 8 of the unit dispatcher at 0x24680 (thiscall, ECX = unit).
// With a unit that has a non-null pointer at +0x10 and the one-shot byte at 0x12339D clear: set the byte and
// call 0x6A0C0(1). Always returns 1.
#include <cstdint>
#include <cstring>
extern "C" uint8_t d2c_24c50(void* unit, uint8_t* flag_12339d, void (*f6a0c0)(int32_t)) {
    uint32_t p = 0;
    if (unit) std::memcpy(&p, (uint8_t*)unit + 0x10, 4);
    if (unit && p && !*flag_12339d) { *flag_12339d = 1; f6a0c0(1); }
    return 1;
}
