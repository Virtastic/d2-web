// 1.13c D2Game+0x298D0: fastcall ECX = dst, EDX = object index n. s = 0xA520(n); if non-null, the C string at s is copied to dst (terminator included); otherwise 0x2A6D(dst, "Object %d", n).
#include "d2game_seam4e.h"
extern "C" void d2g_298d0(uint32_t dst, uint32_t n, const D2Seam* e)
{
    const uint32_t s = e->call(0xA520, 0, 0, 0, 0, 0, n);
    if (s) {
        uint32_t i = 0;
        for (;;) { const uint8_t c = (uint8_t)d2rd8(s + i); d2wr8(dst + i, c); ++i; if (!c) break; }
        return;
    }
    e->call(0x2A6D, 0, 0, 0, 0, 0, dst, 0xB2000000u | 0xFCD28u, n);
}
