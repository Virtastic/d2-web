// 1.13c D2Game+0xEB7B0: quest-record section writer, ECX = pEnd, stdcall (player, ppCursor), ret 8.
// Needs 0x12A bytes (cursor + 0x12A > pEnd -> 1) and the player data from 0xA4C0 (null -> 2). The section is built in a zeroed
// local: 'Woo!' dword, version 6, size word 0x12A, then three 96-byte quest records copied by 0xA43C(rec, dst, 0x60, 1)
// from the three pointers at player data +0x10; the local is copied to the cursor and the cursor advances by 0x12A.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_eb7b0(uint32_t pEnd, uint32_t player, uint32_t ppCursor, const D2Seam* e)
{
    const uint32_t cur = d2rd32(ppCursor);
    if (cur + 0x12A > pEnd) return 1;
    const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, player);
    if (!pd) return 2;
    uint8_t buf[0x12A] = {};
    const uint32_t magic = 0x216F6F57, ver = 6;
    memcpy(buf, &magic, 4);
    memcpy(buf + 4, &ver, 4);
    const uint16_t size = 0x12A;
    memcpy(buf + 8, &size, 2);
    for (int i = 0; i < 3; ++i)
        e->call(0xA43C, 0, 0, 0, 0, 0, d2rd32(pd + 0x10 + 4 * i), (uint32_t)(uintptr_t)(buf + 0xA + 0x60 * i), 0x60, 1);
    memcpy((void*)(uintptr_t)cur, buf, sizeof buf);
    d2wr32(ppCursor, cur + 0x12A);
    return 0;
}
