// 1.13c D2Common+0x3462 __initptd: per-thread data gets the init address and rand seed 1 (offsets 0x54, 0x14). the 1.10f source has no twin.
#include <cstdint>
#include <cstring>

// 1.13c D2Common+0x3462
void d2common_initptd_3462(uint8_t* ptd, uint32_t initAddr)
{
    std::memcpy(ptd + 0x54, &initAddr, 4);
    const uint32_t one = 1;
    std::memcpy(ptd + 0x14, &one, 4);
}
