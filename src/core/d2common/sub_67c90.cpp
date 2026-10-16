// 1.13c D2Common+0x67C90 (#10373)
#include <cstdint>
// Bit test over a word array: bit nBit of the 16-bit words starting at word 1 (table of 0x70 {word index, mask} pairs: word 1 + n/16, mask 1 << n%16).
// Returns the masked word. Out of range (>= 0x70) and a null array are fatal in retail (_exit); we return 0.
uint16_t D2Common_67C90_TestWordBit(const uint16_t* pWords, uint16_t nBit)
{
    if (!pWords || nBit >= 0x70) return 0;
    return (uint16_t)(pWords[1 + nBit / 16] & (1u << (nBit % 16)));
}
