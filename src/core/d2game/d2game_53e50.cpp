// 1.13c D2Game+0x53E50 (D2GAME_SAVE_AttachSaveFile): ecx = client, stack = (game, player, bInteracts), ret 0xC.
// Null client: fatal (assert line 0xB41). A pending save header (client+0x17C) is freed (0x49630, eax = header).
// The player save is built into a 0x2000-byte stack buffer (0xEC7F0: ecx = player, edx = game, stack = buf, &size,
// 0x2000, bInteracts, 0); a size <= 0 is fatal (line 0xB27). The buffer is attached (0x4A190: eax = [client], stack =
// buf, size, size, 0), then client+0x3D4 |= 0x10, +0x184 = 5, +0x188 = 0. Callees go through the D2Seam.
#include <cstdlib>
#include "d2game_seam4e.h"
extern "C" void d2g_53e50(uint32_t client, uint32_t game, uint32_t player, uint32_t bInteracts, const D2Seam* e)
{
    if (!client) abort();   // fatal 0xB41 (Ordinal_10265/10024 then _exit(-1))
    const uint32_t pending = d2rd32(client + 0x17C);
    if (pending) e->call(0x49630, pending);
    uint8_t buf[0x2000];
    uint32_t size = 0;
    e->call(0xEC7F0, 0, player, game, 0, 0, (uint32_t)(uintptr_t)buf, (uint32_t)(uintptr_t)&size, 0x2000, bInteracts, 0);
    if ((int32_t)size <= 0) abort();   // fatal 0xB27
    e->call(0x4A190, d2rd32(client), 0, 0, 0, 0, (uint32_t)(uintptr_t)buf, size, size, 0);
    d2wr32(client + 0x3D4, d2rd32(client + 0x3D4) | 0x10);
    d2wr32(client + 0x184, 5);
    d2wr32(client + 0x188, 0);
}
