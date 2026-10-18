// D2Client+0x075D0 = MSVC 2003 CRT __allmul: low 64 bits of a 64x64 multiply
//. Pure: four stack scalars in, EDX:EAX out, no memory, no callees.
#include <cstdint>

// 1.13c D2Client+0x075D0
int64_t d2_client_allmul(uint32_t lo1, int32_t hi1, uint32_t lo2, int32_t hi2)
{
    if (hi2 == 0 && hi1 == 0) return (int64_t)((uint64_t)lo1 * lo2);   // retail fast path
    const uint64_t x = ((uint64_t)(uint32_t)hi1 << 32) | lo1;
    const uint64_t y = ((uint64_t)(uint32_t)hi2 << 32) | lo2;
    return (int64_t)(x * y);                                           // low 64 bits only
}
