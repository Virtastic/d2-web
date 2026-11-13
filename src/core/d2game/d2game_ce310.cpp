// 1.13c D2Game+0xCE310 -- ECX game, EDX unit, two stack words a0, a1 (ret 8). No result. a0 rides in EAX (and ESI) to the senders; a1 is unused.
// h = 0xAABA(unit): zero ends. 0xA79C(h) with bit 2 of its low byte clear: 0x4AD10(0) equal to unit ends. id = unit[+0xC] (-1 for no unit); 0xE07A0(EAX unit, ECX game);
// t0 = 0xA598(unit[+0x2C]), tgt = 0 when t0 == unit else t0; v = 0xA49C(h, "..\Source\D2Game\PLAYER\PlrMsg.cpp" (0xFB188), 0xCF); b = byte 0xA400(unit, h, 1).
// tgt non-zero and 0x49A50(EAX tgt) non-zero: 0x8D210(EAX a0, EBX 0; 0, id, tgt[0], tgt[+0xC], v, 0, b dword); done. Otherwise the 17-byte packet
// {0x4D, 0, id (dword), v & 0xFFFF (dword), b, 0xAAC6(unit[+0x2C]) word, 0xAAD8(unit[+0x2C]) word, 0 word} goes to 0x8A3E0(EAX a0; &packet, 0x11).
// Stack residue: the b dword's upper bytes in the 0x8D210 call are residue (hookblk mask drops word 6); the packet is fully written.
#include <cstdint>
#include <cstring>
#include "../fn/d2game_gp.h"
namespace d2g {
void fce310(GE& e, uint32_t game, uint32_t unit, uint32_t a0, uint32_t a1)
{
    (void)a1;
    const uint32_t h = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!h) return;
    if (!(e.h(0xA79C, 0, 0, 0, 0, 0, h) & 4)) {
        if (unit == e.h(0x4AD10, 0, 0, 0, 0, 0, 0)) return;
    }
    const uint32_t id = unit ? e.rd32(unit + 0xC) : 0xFFFFFFFFu;
    e.h(0xE07A0, unit, game);
    const uint32_t t0 = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    const uint32_t tgt = (t0 == unit) ? 0 : t0;
    const uint32_t v = e.h(0xA49C, 0, 0, 0, 0, 0, h, 0xB2000000u | 0xFB188u, 0xCF);
    const uint32_t b = e.h(0xA400, 0, 0, 0, 0, 0, unit, h, 1) & 0xFF;
    if (tgt && e.h(0x49A50, tgt)) {
        const uint32_t t00 = e.rd32(tgt), tc = e.rd32(tgt + 0xC);
        const uint32_t w[7] = {0, id, t00, tc, v, 0, 0};
        e.h(0x8D210, a0, 0, 0, 0, 0, 0, id, t00, tc, v, 0, b);
        e.blk(0x8D210, w, 7, 0x3F);
        return;
    }
    const uint32_t s16 = e.h(0xAAD8, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C)) & 0xFFFF;
    const uint32_t c16 = e.h(0xAAC6, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C)) & 0xFFFF;
    uint32_t m[5] = {0, 0, 0, 0, 0};
    uint8_t* p = (uint8_t*)m;
    const uint32_t vl = v & 0xFFFF;
    p[0] = 0x4D; p[1] = 0;
    memcpy(p + 2, &id, 4); memcpy(p + 6, &vl, 4);
    p[10] = (uint8_t)b;
    memcpy(p + 11, &c16, 2); memcpy(p + 13, &s16, 2);
    e.h(0x8A3E0, a0, 0, 0, 0, 0, (uint32_t)(uintptr_t)m, 0x11);
    e.blk(0x8A3E0, m, 5, 0x1F);
}
}
