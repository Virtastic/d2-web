// 1.13c D2Game+0xECA60 -- read the player save's skill section. EAX = unit, EDX = pEnd; stack (ret 8): pCursor, nSkills.
// The section is the "if" marker (0x6669) and one byte per class skill slot. Returns 0x13 when the marker is missing or past pEnd,
// the unit is not a player, or its player data has no client; otherwise 0 with the cursor moved past marker and bytes.
// Callees: 0xA4C0 (player data), 0x49BF0 (inline here: the client's class byte at +8), 0xA442 / 0xA418 (class skill id / count),
// 0x9F9A0 (EAX = skill id, ESI = unit; level, 1), 0x8B5A0 (client, unit), 0xAEB6 (unit), 0x96FA0 (unit +0x80, unit).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_eca60(uint32_t unit, uint32_t pEnd, uint32_t pCur, uint32_t n, const D2Seam* e)
{
    const uint32_t cur = d2rd32(pCur);
    const uint32_t body = cur + 2;
    if (body > pEnd || d2rd16(cur) != 0x6669) return 0x13;
    if (!unit || d2rd32(unit) != 0) return 0x13;
    const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t client = d2rd32(pd + 0x9c);
    if (!client) return 0x13;
    const uint32_t cls = d2rd8(client + 8);
    for (int32_t i = 0; i < (int32_t)n; ++i) {
        const uint32_t id = e->call(0xA442, 0, 0, 0, 0, 0, cls, (uint32_t)i);
        const int32_t cnt = (int32_t)e->call(0xA418, 0, 0, 0, 0, 0, cls);
        if (i > cnt) break;
        const uint32_t lvl = d2rd8(body + (uint32_t)i);
        if (lvl) e->call(0x9F9A0, id, 0, 0, 0, unit, lvl, 1);
    }
    e->call(0x8B5A0, 0, 0, 0, 0, 0, client, unit);
    e->call(0xAEB6, 0, 0, 0, 0, 0, unit);
    if (d2rd32(unit) == 0) e->call(0x96FA0, 0, 0, 0, 0, 0, d2rd32(unit + 0x80), unit);
    d2wr32(pCur, body + n);
    return 0;
}
