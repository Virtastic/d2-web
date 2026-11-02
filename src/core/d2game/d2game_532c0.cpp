// 1.13c D2Game+0x532C0: ebx = cost, two stack words (ret 8): a0 = object (+0x70 holds the owner pointer), a1 = stat id (full dword for #52A20, low word for #A3BE). #52A20 is a register callee (eax = level, ecx = stat id) returning a rate; the cost grows level by level (rate * (level+1) * level * 3, unsigned 32-bit) until it passes ebx or level 99; #A3BE = Ordinal_11156 (owner, stat word, level), #A3A6 = Ordinal_10448 (level, entry+0x20). Returns the larger (unsigned) of ebx and #A3A6, or ebx when #A3BE finds no entry.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_532c0(uint32_t ebx, uint32_t a0, uint32_t a1, const D2Seam* e)
{
    uint32_t n = 1;
    uint32_t t = e->call(0x52A20, 2, a1);
    if (t * 18u <= ebx) {
        int32_t lv = 2;
        while (lv < 0x63) {
            ++n; ++lv;
            t = e->call(0x52A20, (uint32_t)lv, a1);
            if (t * (uint32_t)(lv + 1) * (uint32_t)lv * 3u > ebx) break;
        }
    }
    const uint32_t ent = e->call(0xA3BE, 0, 0, 0, 0, 0, d2rd32(a0 + 0x70), a1 & 0xFFFF, n);
    if (!ent) return ebx;
    const uint32_t v = e->call(0xA3A6, 0, 0, 0, 0, 0, n, d2rd32(ent + 0x20));
    return v > ebx ? v : ebx;
}
