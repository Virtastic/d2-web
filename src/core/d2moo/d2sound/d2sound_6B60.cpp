#include <cstdint>
extern int32_t DAT_6f9c658c, DAT_6f9c5994; // host seams
// 1.13c D2sound+0x6B60 returns DAT_6f9c5994 when DAT_6f9c658c is nonzero, else 0.
uint32_t D2sound_6B60() { return DAT_6f9c658c ? (uint32_t)DAT_6f9c5994 : 0; }
