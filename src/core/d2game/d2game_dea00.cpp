// D2Game+0xDEA00 -- hand one item's stat amounts to the unit it belongs to. Stack a0 = game, a1 = unit (type 0 or 1), a2 = item, ret 0xC (void). Written from the 1.13c
// disassembly. n = stat(item, 0xD) must be > 0; esi = 0xDB5E0(EBX = a1; game, item) must be nonzero; v12 = stat(item, 0xC). When the data-table count at
// [[D2Game+0xF829C]] + 0xBF0 is > 7 and esi's record (0xA4C0) lists a first unflagged entry whose class names an item b of the game's table (game + 0x1320 + (class & 0x7F) * 4,
// chained through +0xE4), then 0xDDCB0(EAX = 0xDC030(EAX = n, EBX = b; game, v12) scaled by 0x56 / 256 unless a1 == b; game, esi, b, stat(b, 0xC)) runs. Then edi = stat(esi, 0xC):
// 0xDFA60(EAX = esi) != 0xFFFF (16 bit) takes 0xDE8F0(EAX = esi; game, item, edi, v12, n), else 0xDE6D0(EAX = esi; game, edi, 0xDC030(EAX = n, EBX = esi; game, v12)).
// A null game with an entry found is a fatal path (Fog 0xA2DA / 0xA2C2) and is not modelled. gTables = the address of the word holding the D2Game+0xF829C slot value.
#include "d2game_seam4e.h"
// 1.13c D2Game+0xDEA00
extern "C" void d2game_dea00(uint32_t game, uint32_t unit, uint32_t item, uint32_t gTables, const D2Seam* e)
{
    if (!unit || !item) return;
    const uint32_t t = d2rd32(unit);
    if (t != 0 && t != 1) return;
    const int32_t n = (int32_t)e->call(0xA3E2, 0, 0, 0, 0, 0, item, 0xD, 0);
    if (n <= 0) return;
    const uint32_t esi = e->call(0xDB5E0, 0, 0, 0, unit, 0, game, item);
    if (!esi) return;
    const uint32_t v12 = e->call(0xA3E2, 0, 0, 0, 0, 0, item, 0xC, 0);
    const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, esi);
    if (pd && (int32_t)d2rd32(d2rd32(d2rd32(gTables)) + 0xBF0) > 7) {
        uint32_t head = 0;
        const uint32_t q = d2rd32(pd + 0x44);
        if (q) {
            const uint32_t rr = d2rd32(q) + 0x54;
            head = rr ? d2rd32(rr) : 0;
            while (head && (d2rd32(head) & 1)) head = d2rd32(head + 0x14);   // test byte [eax], 1
        }
        if (head) {
            const uint32_t cls = d2rd32(head + 4);
            uint32_t b = d2rd32(game + 0x1320 + (cls & 0x7F) * 4);
            while (b && d2rd32(b + 0xC) != cls) b = d2rd32(b + 0xE4);
            if (b) {
                const uint32_t edi = e->call(0xA3E2, 0, 0, 0, 0, 0, b, 0xC, 0);
                int32_t val = (int32_t)e->call(0xDC030, (uint32_t)n, 0, 0, b, 0, game, v12);
                if (unit != b) {
                    const int32_t p = (int32_t)((uint32_t)val * 0x56u);
                    val = (p + (int32_t)((uint32_t)(p >> 31) & 0xFFu)) >> 8;
                }
                e->call(0xDDCB0, (uint32_t)val, 0, 0, 0, 0, game, esi, b, edi);
            }
        }
    }
    const uint32_t edi = e->call(0xA3E2, 0, 0, 0, 0, 0, esi, 0xC, 0);
    if ((e->call(0xDFA60, esi) & 0xFFFFu) != 0xFFFFu) {
        e->call(0xDE8F0, esi, 0, 0, 0, 0, game, item, edi, v12, (uint32_t)n);
        return;
    }
    const uint32_t res = e->call(0xDC030, (uint32_t)n, 0, 0, esi, 0, game, v12);
    e->call(0xDE6D0, esi, 0, 0, 0, 0, game, edi, res);
}
