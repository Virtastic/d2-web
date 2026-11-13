// 1.13c D2Client+0xAC0E0: clears ten dwords of a module table, 16 bytes apart from D2Client+0x119685 (an unaligned base). Returns 0.
#include <cstdint>
#include <cstring>
extern "C" uint32_t d2c_ac0e0(uint8_t* g /* the 148 bytes at D2Client+0x119685 */) {
    for (int k = 0; k < 10; k++) std::memset(g + 16 * k, 0, 4);
    return 0;
}
