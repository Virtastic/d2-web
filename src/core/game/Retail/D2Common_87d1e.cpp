// CRT 96-bit shift right by one, moved from the stand-alone d2common batch into the game tree.
#include <cstdint>

// 1.13c D2Common+0x87D1E
void d2common_shr_12_87d1e(uint32_t* words)
{
    const uint32_t middle = words[1];
    words[1] = (middle >> 1) | (words[2] << 31);
    words[2] >>= 1;
    words[0] = (words[0] >> 1) | (middle << 31);
}
