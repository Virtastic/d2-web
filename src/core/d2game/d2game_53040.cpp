// 1.13c D2Game+0x53040: eax = session, edx = src block (0x50 bytes), stack a0 = out (ret 4). *out starts at 0 and 0xA4C0(session) supplies a
// table at once. The block must start with the word 0x5357, else 3. Three 0x18-byte slots from +8 each begin with a word that is 0, 0x101
// or 0x102 (anything else returns 3 at once); a slot goes to 0xA47E(table[0x1C+4*i], slot). On completion *out = 0x50 and the result is 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_53040(uint32_t sess, uint32_t src, uint32_t out, const D2Seam* e)
{
    d2wr32(out, 0);
    uint8_t buf[0x50];
    memcpy(buf, (const void*)(uintptr_t)src, sizeof buf);
    const uint32_t b = (uint32_t)(uintptr_t)buf;
    const uint32_t tb = e->call(0xA4C0, 0, 0, 0, 0, 0, sess);
    if (d2rd16(b) != 0x5357) return 3;
    for (uint32_t i = 0; i < 3; i++) {
        const uint32_t p = b + 8 + 0x18 * i;
        const uint32_t w = d2rd16(p);
        if (w != 0x101 && w != 0x102 && w != 0) return 3;
        e->call(0xA47E, 0, 0, 0, 0, 0, d2rd32(tb + 0x1C + 4 * i), p);
        e->blk(0xA47E, (const void*)(uintptr_t)p, 6);
    }
    d2wr32(out, 0x50);
    return 0;
}
