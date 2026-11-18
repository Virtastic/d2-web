#include <cstdint>

// 0x9330, 0x87D1E and 0xDB40 live in the game tree (src/core/d2moo/src/Retail).

// 1.13c D2Common+0x87CF0
void d2common_shl_12_87cf0(uint32_t* words)
{
    const uint32_t low = words[0];
    const uint32_t middle = words[1];
    words[0] = low << 1;
    words[1] = (middle << 1) | (low >> 31);
    words[2] = (words[2] << 1) | (middle >> 31);
}
