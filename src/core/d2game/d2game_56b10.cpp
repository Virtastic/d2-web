// 1.13c D2Game+0x56B10: ecx = quest node. State (+0xC) < 5 with the +9 flag set returns 1 (state 0 is promoted to 1); otherwise
// node2 = #33F30(eax = node[4], ecx = node[0x10]); none returns 0, else the result of the handler at node2+0xF0 (ecx = node2; IsBadCodePtr-guarded in retail).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_56b10(uint32_t n, const D2Seam* e)
{
    const uint32_t st = d2rd8(n + 0xC);
    if (st < 5 && d2rd8(n + 9)) { if (!st) d2wr8(n + 0xC, 1); return 1; }
    const uint32_t p = e->call(0x33F30, d2rd32(n + 4), d2rd32(n + 0x10));
    if (!p) return 0;
    return e->call(0x56E00, 0, p) & 0xFF;
}
