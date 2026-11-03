// 1.13c D2Game+0x93040: ecx = this, edx = p (plain ret, always returns 1). this+0 = x0, this+4 = unit, this+0xC = B; the 0x70-byte request
// block at [B] is copied to a local; B+4 / B+8 (word) / B+0xC / B+0x10 drive it. A clear bit 0 in word B+8 runs 0xDE5A0(eax = p; x0, B+4, 1) into the word at +4 (its bit 0
// ends the call when clear; else the B+8 word is ORed in); a set bit 0 takes the B+8 word as is. With bit 0 set the block's dword 0 ORs B+0xC, a nonzero byte B+0x10 runs
// 0xDD450(x0, unit, p, &block, 0, byte), then 0xDDDE0(eax = unit, cl = 0x80 under x0's upper bits, edi = &block; x0, p) and 0xDED30(x0, unit, p) close it.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_93040(uint32_t t, uint32_t p, const D2Seam* e)
{
    const uint32_t B = d2rd32(t + 0xC), x0 = d2rd32(t), un = d2rd32(t + 4);
    uint32_t q[28];
    for (int i = 0; i < 28; i++) q[i] = d2rd32(d2rd32(B) + 4 * i);
    const uint32_t w8 = d2rd16(B + 8);
    if (!(w8 & 1)) {
        const uint32_t ax = e->call(0xDE5A0, p, 0, 0, 0, 0, x0, d2rd32(B + 4), 1) & 0xFFFF;
        q[1] = (q[1] & 0xFFFF0000u) | ax;
        if (!(ax & 1)) return 1;
        q[1] = (q[1] & 0xFFFF0000u) | ((q[1] & 0xFFFF) | w8);
    } else {
        q[1] = (q[1] & 0xFFFF0000u) | w8;
    }
    if (!(q[1] & 1)) return 1;
    q[0] |= d2rd32(B + 0xC);
    const uint32_t b = d2rd8(B + 0x10);
    if (b) {
        e->call(0xDD450, 0, 0, 0, 0, 0, x0, un, p, (uint32_t)(uintptr_t)q, 0);
        e->blk(0xDD450, q, 28);
    }
    e->call(0xDDDE0, un, (x0 & ~0xFFu) | 0x80, 0, 0, 0, x0, p);
    e->blk(0xDDDE0, q, 9);
    e->call(0xDED30, 0, 0, 0, 0, 0, x0, un, p);
    return 1;
}
