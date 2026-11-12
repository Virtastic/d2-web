// 1.13c D2Game+0xEC020: 'fk' iron golem section, EAX = game, stdcall (player, ppCursor, pEnd, -, -), ret 0x14.
// A game without +0x70 writes nothing (0). Otherwise 'fk' goes to the cursor, then the player's golem is looked up with
// 0x96A40 (EAX = player, EBX = 1, ESI = 3). When it is unit class 0x123 and the player has skill 0x5A (0xA3D0, ECX = player,
// EDX = 0x5A, -1), the golem's +0x60 item goes through 0xA3DC and 0xA46C; a non-null result makes the flag byte 1 and the item
// is written behind it by 0x53470 (EAX = result, EDX = bytes left, one stack word = destination). A write of <= 0 bytes clears
// the flag byte. The cursor advances over the flag byte and what was written.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ec020(uint32_t game, uint32_t player, uint32_t ppCursor, uint32_t pEnd, uint32_t, uint32_t, const D2Seam* e)
{
    uint32_t out = d2rd32(ppCursor);
    if (!d2rd32(game + 0x70)) return 0;
    d2wr16(out, 0x666B);
    out += 2;
    const uint32_t golem = e->call(0x96A40, player, 0, 0, 1, 3);
    uint32_t item = 0, flag = 0;
    if (golem && d2rd32(golem + 4) == 0x123 && e->call(0xA3D0, 0, player, 0x5A, 0, 0, 0xFFFFFFFFu)) {
        const uint32_t inv = d2rd32(golem + 0x60);
        if (inv) {
            const uint32_t it = e->call(0xA3DC, 0, 0, 0, 0, 0, inv);
            if (it) {
                item = e->call(0xA46C, 0, 0, 0, 0, 0, it);
                flag = item ? 1 : 0;
            }
        }
    }
    const uint32_t pos = out;
    d2wr8(out++, flag);
    if (flag) {
        const int32_t n = (int32_t)e->call(0x53470, item, 0, pEnd - out, 0, 0, out);
        if (n <= 0) d2wr8(pos, 0);
        out += (uint32_t)n;
    }
    d2wr32(ppCursor, out);
    return 0;
}
