// 1.13c D2Game+0xECC90: PLRSAVE2_InitializeStats. ESI = unit; stack (ret 0xC): s0 (unused), hitpoints, mana.
// Clamps gold and stash gold, sets stamina, re-maps the 16 hotkey item words and the four skill-flag words through 0xEB8B0
// (EAX = unit, stack word = value), assigns the saved left skill, runs the right-skill setter, writes hitpoints, mana and
// the three 100 rates, the class stat 0x1e, and sends the 0x97 packet when the client's switch byte is set.
// Retail's EAX at the return is the client pointer (no 0x97 send) or the send's result.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ecc90(uint32_t unit, uint32_t s0, uint32_t hp, uint32_t mana, const D2Seam* e)
{
    (void)s0;
    uint32_t cl = 0;
    if (unit && d2rd32(unit) == 0) cl = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9c);
    const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);

    // gold (stat 0xe) and stash gold (stat 0xf): out of [0, limit] -> 0
    int32_t v = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xe, 0);
    // the limit is only read when the amount is non-negative (retail's `-1 < v` guard short-circuits it)
    int32_t lim = v >= 0 ? (int32_t)e->call(0xA45A, 0, 0, 0, 0, 0, unit) : 0;
    if (v < 0 || v > lim) {
        if (unit) {
            if (d2rd32(unit) == 0) e->call(0xA45A, 0, 0, 0, 0, 0, unit);   // the clamp path asks for the limit a second time (player units)
            e->call(0xA472, 0, 0, 0, 0, 0, unit, 0xe, 0, 0);
        }
    }
    v = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xf, 0);
    lim = v >= 0 ? (int32_t)e->call(0xA4A8, 0, 0, 0, 0, 0, unit) : 0;
    if (v < 0 || v > lim) {
        if (unit) {
            if (d2rd32(unit) == 0) e->call(0xA4A8, 0, 0, 0, 0, 0, unit);
            e->call(0xA472, 0, 0, 0, 0, 0, unit, 0xf, 0, 0);
        }
    }
    e->call(0xA472, 0, 0, 0, 0, 0, unit, 0xa, e->call(0xA460, 0, 0, 0, 0, 0, unit), 0);

    // 16 hotkey slots at client+0x3dc, stride 8: word, byte, item dword. Without a client, retail's registers keep the
    // hitpoints argument (eax) and no slot is read or written.
    uint32_t item = hp;   // eax carries across iterations when there is no client
    for (uint32_t i = 0; i < 16; i++) {
        if (cl) item = d2rd32(cl + 0x3de + 8 * i + 2);
        if (item) {
            item = e->call(0xEB8B0, unit, 0, 0, 0, 0, item);
            if (cl) d2wr32(cl + 0x3de + 8 * i + 2, item);
        }
    }
    static const uint32_t fo[4] = { 0x7c, 0x78, 0x8c, 0x88 };
    for (int i = 0; i < 4; i++) {
        const uint32_t w = d2rd32(pd + fo[i]);
        if (w) d2wr32(pd + fo[i], e->call(0xEB8B0, unit, 0, 0, 0, 0, w));
    }

    const uint32_t skill = d2rd32(pd + 0x74);
    if (skill && skill != 5) {
        const uint32_t flags = d2rd32(pd + 0x7c);
        const uint32_t sk = e->call(0xA3D0, 0, unit, skill, 0, 0, flags);
        if (sk) {
            e->call(0xADF0, 0, 0, 0, 0, 0, unit, skill, flags);
            uint32_t slot = 0xffffffffu;
            e->call(0xAE8C, 0, 0, 0, 0, 0, sk, (uint32_t)(uintptr_t)&slot, 0, 0, 0);
            if (unit && d2rd32(unit) == 0) {
                const uint32_t id = d2rd32(unit + 0xc);
                const uint32_t c2 = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9c);
                uint8_t pkt[16] = { 0 };
                pkt[0] = 0x23; pkt[1] = (uint8_t)d2rd32(unit); pkt[6] = 1;
                memcpy(pkt + 2, &id, 4); memcpy(pkt + 7, &skill, 2); memcpy(pkt + 9, &slot, 4);
                e->call(0x8A3E0, c2, 0, 0, 0, 0, (uint32_t)(uintptr_t)pkt, 13);
                e->blk(0x8A3E0, pkt, 4);
            }
        }
    }
    const uint32_t right = d2rd32(pd + 0x70);
    if (right) e->call(0xA2100, 0, unit, 0, 0, 0, right, d2rd32(pd + 0x78));

    e->call(0xA472, 0, 0, 0, 0, 0, unit, 6, hp, 0);
    e->call(0xA472, 0, 0, 0, 0, 0, unit, 8, mana, 0);
    e->call(0xA472, 0, 0, 0, 0, 0, unit, 0x44, 0x64, 0);
    e->call(0xA472, 0, 0, 0, 0, 0, unit, 0x43, 0x64, 0);
    e->call(0xA472, 0, 0, 0, 0, 0, unit, 0x45, 0x64, 0);
    const uint32_t cls = unit ? d2rd32(unit + 4) : 0xffffffffu;
    const uint32_t a = e->call(0xA3E2, 0, 0, 0, 0, 0, unit, 0xc, 0);
    e->call(0xA472, 0, 0, 0, 0, 0, unit, 0x1e, e->call(0xA3D6, 0, 0, 0, 0, 0, cls, a), 0);

    uint32_t ret = cl;
    if (d2rd8(cl + 0x45c)) {
        uint8_t pkt[16] = { 0x97 };
        ret = e->call(0x8A3E0, cl, 0, 0, 0, 0, (uint32_t)(uintptr_t)pkt, 1);
        e->blk(0x8A3E0, pkt, 4);
    }
    return ret;
}
