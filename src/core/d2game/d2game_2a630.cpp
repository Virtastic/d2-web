// 1.13c D2Game+0x2A630: fastcall ECX = dst, EDX = monster index n. In range of dt+0xA80 with a row (dt+0xA78, 0x1A8 bytes): dst receives the D2Lang conversion (import slot 0xF8B40, fastcall dst, string, 0x40) of 0xB462(row word +6); otherwise 0x2A6D(dst, "Monster %d", n).
#include "d2game_seam4e.h"
extern "C" void d2g_2a630(uint32_t dst, uint32_t n, uint32_t dt, const D2Seam* e)
{
    if ((int32_t)n >= 0 && (int32_t)n < (int32_t)d2rd32(dt + 0xA80)) {
        const uint32_t row = n * 0x1A8u + d2rd32(dt + 0xA78);
        if (row) {
            const uint32_t s = e->call(0xB462, 0, 0, 0, 0, 0);
            e->call(0xF8B40, 0, dst, s, 0, 0, 0x40);
            return;
        }
    }
    e->call(0x2A6D, 0, 0, 0, 0, 0, dst, 0xB2000000u | 0xFCD34u, n);
}
