// 1.13c D2Game+0x29890: fastcall ECX = dst, EDX = item index n. Record = 0xA53E(n); if present, dst receives the D2Lang conversion (import slot 0xF8B40, fastcall dst, string, 0x40) of the string 0xB462(record word +0xF4); otherwise the log printf at 0x2A6D(dst, "Item %d", n).
#include "d2game_seam4e.h"
extern "C" void d2g_29890(uint32_t dst, uint32_t n, const D2Seam* e)
{
    const uint32_t rec = e->call(0xA53E, 0, 0, 0, 0, 0, n);
    if (rec) {
        const uint32_t s = e->call(0xB462, 0, 0, 0, 0, 0);
        e->call(0xF8B40, 0, dst, s, 0, 0, 0x40);
        return;
    }
    e->call(0x2A6D, 0, 0, 0, 0, 0, dst, 0xB2000000u | 0xFCD20u, n);
}
