// 1.13c D2Game+0x58260: eax = unit (null gives id -1, else unit+0xC), ecx = quest node; scans the 3 rows x 5 dwords at (node+0x18)+0xB0 for the id; returns the row index or -1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_58260(uint32_t unit, uint32_t q)
{
    const uint32_t id = unit ? d2rd32(unit + 0xC) : 0xFFFFFFFFu;
    uint32_t p = d2rd32(q + 0x18) + 0xB0;
    for (uint32_t row = 0; row < 3; ++row, p += 0x14)
        for (uint32_t c = 0; c < 5; ++c)
            if (id == d2rd32(p + c * 4)) return row;
    return 0xFFFFFFFFu;
}
