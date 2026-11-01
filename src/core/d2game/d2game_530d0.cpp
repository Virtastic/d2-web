// 1.13c D2Game+0x530D0: edx = src block, stack a0 = session, a1 = out (ret 8). *out starts at 0. Copies 0x12A bytes; when the block starts with the
// 0x216f6f57 magic and its word at +4 is 6, asks 0xA4C0(session) for a table and feeds each of three 0x60-byte chunks (copy+0xA+0x60*i)
// to 0xA3FA(table[0x10+4*i], chunk, 0x60, 1); then *out = 0x12A and returns 0. Any other block returns 2.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_530d0(uint32_t src, uint32_t a0, uint32_t out, const D2Seam* e)
{
    d2wr32(out, 0);
    uint8_t buf[0x12A];
    memcpy(buf, (const void*)(uintptr_t)src, sizeof buf);
    const uint32_t b = (uint32_t)(uintptr_t)buf;
    if (d2rd32(b) != 0x216f6f57 || d2rd32(b + 4) != 6) return 2;
    const uint32_t tb = e->call(0xA4C0, 0, 0, 0, 0, 0, a0);
    for (uint32_t i = 0; i < 3; i++) {
        const uint32_t p = b + 0xA + 0x60 * i;
        e->call(0xA3FA, 0, 0, 0, 0, 0, d2rd32(tb + 0x10 + 4 * i), p, 0x60, 1);
        e->blk(0xA3FA, (const void*)(uintptr_t)p, 24);
    }
    d2wr32(out, 0x12A);
    return 0;
}
