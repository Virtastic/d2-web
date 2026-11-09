// 1.13c D2Game+0xEBAF0 -- save-header join check. EAX = flags, ESI = game, stack client record (+400 a FILETIME), ret 4.
// Seam form of d2game_ebaf0.cpp (same rule): g40 points at the pinned DAT_6FD31C40 word; 0xB23A0 is the only logged callee.
// Returns 0 on success, else the join-validation error code.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ebaf0(uint32_t flags, uint32_t game, uint32_t client, uint32_t g40, const D2Seam* e)
{
    const bool b20 = (flags & 0x20) != 0;
    if (!b20) { if (d2rd32(game + 0x70)) return 9; }
    else if (!d2rd32(game + 0x70)) return 8;
    if (d2rd32(g40)) {
        uint32_t now[2] = {};
        e->call(0x111C3C, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)now);   // (*(DAT_6FD31C3C + 0x54))(&now)
        const int64_t a = (int64_t)(((uint64_t)d2rd32(client + 0x194) << 32) | d2rd32(client + 0x190));
        const int64_t n = (int64_t)(((uint64_t)now[1] << 32) | now[0]);
        if ((flags & 0x40) != 0 && a > n) { if (!d2rd32(game + 0x74)) return 0x19; }
        else if (d2rd32(game + 0x74)) return 0x1a;
    }
    if (!(flags & 4)) {
        const uint32_t inner = d2rd32(game + 0x1D28);
        if (!inner) {
            const uint32_t s = e->call(0xA2DA, 0, 0, 0, 0, 0, 0x139);
            e->call(0xA2C2, 0, 0, 0, 0, 0, 0x6FD19FF9, s);   // then _exit(-1)
            return 0;
        }
        if (d2rd32(inner + 8) & 0x800) return 0xC;
    } else {
        if (flags & 8) return 10;
        if (!(e->call(0xB23A0, game) & 0x800)) return 0xB;
    }
    const uint32_t u = flags >> 8 & 0x1F, m = d2rd8(game + 0x6D);
    if (m < 2) {
        if (m && ((b20 && u < 5) || u < 4)) return 0xD;
    } else if ((b20 && u < 10) || u < 8) return 0xE;
    return 0;
}
