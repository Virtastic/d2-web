// 1.13c D2Game+0x58BE0: eax = unit (first arg of the setter calls), ecx = ctx unit pointer (its type dword must be 0; retail reads +0xA of a null record otherwise), ebx = game (+0x6D byte index, +0x10F4 quest list head). #A57A = Ordinal_10003 (setter); #A4C0 = Ordinal_10920 (record at result +0x9C); the +0xA word bit field is updated (idx*a+5, a = ((w&0x20)|0x80)>>5); then quest 0x24's data +0x88 is incremented.
// Returns retail's EAX at exit (the function is void; the FN record compares it): quest 0x24's data pointer, else 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_58be0(uint32_t unit, uint32_t cu, uint32_t game, const D2Seam* e)
{
    e->call(0xA57A, 0, 0, 0, 0, 0, unit, 0x28, 0xD);
    e->call(0xA57A, 0, 0, 0, 0, 0, unit, 0x28, 0);
    uint32_t rec = 0;
    if (cu && d2rd32(cu) == 0) rec = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, cu) + 0x9C);
    const uint32_t idx = d2rd8(game + 0x6D);
    const uint32_t w = d2rd16(rec + 0xA);
    const uint32_t a = ((w & 0x20) | 0x80) >> 5;
    const int32_t v = (int32_t)(a * idx + 5);
    if (v >= (int32_t)((w >> 8) & 0x1F)) d2wr16(rec + 0xA, ((uint32_t)v << 8) | (w & 0xE0FF));
    const uint32_t hdr = d2rd32(game + 0x10F4);
    if (!hdr) return 0;
    for (uint32_t n = d2rd32(hdr); n; n = d2rd32(n + 0xF4))
        if (d2rd32(n) == 0x24) { const uint32_t qd = d2rd32(n + 0x18); d2wr32(qd + 0x88, d2rd32(qd + 0x88) + 1); return qd; }
    return 0;
}
