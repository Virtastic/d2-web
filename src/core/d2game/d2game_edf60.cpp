// 1.13c D2Game+0xEDF60: stdcall (room, pX, pY), ret 0xC. Free spot next to a random entry of the room's neighbour list.
// 0xB1F8(room, &count) returns the list (stride 0x30: +0x8/+0xC offsets, +0x18 the entry's room); count 0 or no list -> 0.
// The start index comes from 0x11D0 (ECX = room+0x6C or 0, ESI = count; 0 -> 1) and the loop runs to start-1, stepping
// (i+1) % count. 0xA6D0(room, &box) fills six dwords; box[3]/box[4] are the origin added to the entry offsets. An entry whose
// room has bit 2 in 0xB45C(entryRoom) gives the candidate (5*(+0x8+box[3])+3, 5*(+0xC+box[4])+3); 0xAFE8(room,x,y,0x100) must
// answer 0 and then one of the four (dx,dy) pairs at D2Game+0xFC278 must answer 0 from 0xAEE6(room,x+dx,y+dy,2,0x1C09):
// the candidate goes to *pX/*pY and the answer is 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_edf60(uint32_t room, uint32_t pX, uint32_t pY, const D2Seam* e)
{
    uint32_t cnt[2] = { 0, 0 };
    const uint32_t list = e->call(0xB1F8, 0, 0, 0, 0, 0, room, (uint32_t)(uintptr_t)cnt);
    if (!list) return 0;
    const uint32_t n = cnt[0];
    if (!n) return 0;
    uint32_t start = e->call(0x11D0, 0, room ? room + 0x6C : 0, 0, 0, n);
    if (!start) start = 1;
    uint32_t box[6] = { 0, 0, 0, 0, 0, 0 };
    e->call(0xA6D0, 0, 0, 0, 0, 0, room, (uint32_t)(uintptr_t)box);
    const uint32_t end = start - 1;
    static const int32_t kOfs[4][2] = { { 0, -3 }, { 3, 0 }, { 0, 3 }, { -3, 0 } };   // D2Game+0xFC278
    uint32_t i = start;
    do {
        const uint32_t ent = list + i * 0x30;
        const uint32_t er = d2rd32(ent + 0x18);
        if (er && (e->call(0xB45C, 0, 0, 0, 0, 0, er) & 2)) {
            // retail (vector): the origin is box[4]/box[5] (the decompile's iStack_20/1c sit 0x10 bytes into the 0xA6D0 out block)
            const uint32_t x = (d2rd32(ent + 8) + box[4]) * 5 + 3;
            const uint32_t y = (d2rd32(ent + 0xC) + box[5]) * 5 + 3;
            if ((uint16_t)e->call(0xAFE8, 0, 0, 0, 0, 0, room, x, y, 0x100) == 0) {
                for (int k = 0; k < 4; ++k)
                    if ((uint16_t)e->call(0xAEE6, 0, 0, 0, 0, 0, room, x + (uint32_t)kOfs[k][0], y + (uint32_t)kOfs[k][1], 2, 0x1C09) == 0) {
                        d2wr32(pX, x);
                        d2wr32(pY, y);
                        return 1;
                    }
            }
        }
        i = (i + 1) % n;
    } while (i != end);
    return 0;
}
