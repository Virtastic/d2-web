// 1.13c D2Game+0xEBC40: 'fi' skill-level section, EBX = player, stdcall (ppCursor, pEnd), ret 8. dt = *sgptDataTables (the
// descriptor pins the import slot); dt+0xBA8 is the class-skill count the section needs, plus the two marker bytes
// (cursor + count + 2 > pEnd -> 1). The marker is written before the unit is checked, as in 1.13c. The player must be a
// player unit with a client (+0x9C of the 0xA4C0 data), else the fatal path (0x8A4); a client class byte (+8) >= 7 -> 2 with
// the cursor unmoved. One byte per class skill: 0xA400(player, 0xA3D0(player, 0xA442(class, i), -1), 0), the count re-read
// from 0xA418(class) on every pass; the cursor advances over the bytes written.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ebc40(uint32_t player, uint32_t ppCursor, uint32_t pEnd, uint32_t dt, const D2Seam* e)
{
    uint32_t out = d2rd32(ppCursor);
    if (out + d2rd32(dt + 0xBA8) + 2 > pEnd) return 1;
    d2wr16(out, 0x6669);                                          // 'fi'
    out += 2;
    uint32_t client = 0;
    if (player && d2rd32(player) == 0) client = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, player) + 0x9C);
    if (!client) {                                                // D2_ASSERT, line 0x8A4: Fog message then exit(-1)
        const uint32_t t = e->call(0xA2DA, 0, 0, 0, 0, 0, 0x8A4);
        e->call(0xA2C2, 0, 0, 0, 0, 0, 0x6FD19FF9u, t);
        e->call(0x2A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
        return 0;
    }
    const uint32_t cls = d2rd8(client + 8);
    if (cls >= 7) return 2;
    int32_t i = 0;
    if ((int32_t)e->call(0xA418, 0, 0, 0, 0, 0, cls) > 0) {
        do {
            const uint32_t id = e->call(0xA442, 0, 0, 0, 0, 0, cls, (uint32_t)i);
            const uint32_t skill = e->call(0xA3D0, 0, player, id, 0, 0, 0xFFFFFFFFu);
            d2wr8(out++, e->call(0xA400, 0, 0, 0, 0, 0, player, skill, 0));
            ++i;
        } while (i < (int32_t)e->call(0xA418, 0, 0, 0, 0, 0, cls));
    }
    d2wr32(ppCursor, out);
    return 0;
}
