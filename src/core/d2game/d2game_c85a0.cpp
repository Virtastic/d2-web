// D2Game ACT2Q4 scroll-message callback (quest callback slot 11), over retail-layout (32-bit) memory. Written from the 1.13c disassembly.
// Convention: ECX = quest record, EDX = quest arg (+0 game, +8 arg word, +0xC player, +0x14 NPC class word, +0x18 message word); plain ret.
// Record: +4 game, +9 active, +0xB state, +0x14 state byte, +0x18 extension block, +0x1C GUID list, +0xE0 quest id.
// Extension: +4 activated, +8 stored room, +0x16 canyon portal open.
// Callees are D2Seam calls with retail argument placement; 0xDFCC0 takes EDI = game, carried in the ESI slot of the seam.
// The act-mismatch and null-callback asserts (error report plus exit in retail) are never reached by valid input and return here.
#include "d2game_seam4e.h"

static inline uint32_t mod(uint32_t rva) { return 0xB2000000u | rva; }

// 1.13c D2Game+0xC85A0
extern "C" void d2game_c85a0(uint32_t qd, uint32_t arg, const D2Seam* e)
{
    const uint16_t cls = (uint16_t)d2rd16(arg + 0x14);
    const uint16_t msg = (uint16_t)d2rd16(arg + 0x18);
    if (!qd) return;
    uint32_t ext = d2rd32(qd + 0x18);
    const uint32_t game = d2rd32(arg + 0);
    const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, d2rd32(arg + 0xC));
    const uint32_t rec = d2rd32(pd + 0x10 + 4 * d2rd8(game + 0x6D));
    if (cls == 0x14B) {
        if (msg == 0xBA) { e->call(0xA57A, 0, 0, 0, 0, 0, rec, 0xC, 7); return; }
        if (msg > 0xBA && msg <= 0xBD) e->call(0xA57A, 0, 0, 0, 0, 0, rec, 0xC, 8);
        return;
    }
    if (msg == 0x196 || msg == 0x193 || msg == 0x18D || msg == 0x190 || msg == 0x18E || msg == 0x18F ||
        msg == 0x195 || msg == 0x197 || msg == 0x192 || msg == 0x191 || msg == 0x194) {
        e->call(0x35910, d2rd32(arg + 8), d2rd32(arg + 0xC), 0, 0, 0, d2rd32(arg + 0));
        if (e->call(0xA568, 0, 0, 0, 0, 0, rec, 0xC, 1)) {
            e->call(0xA586, 0, 0, 0, 0, 0, rec, 0xC, 1);
            const uint32_t pl = d2rd32(arg + 0xC);
            e->call(0x33880, qd + 0x1C, 0, 0, 0, pl ? d2rd32(pl + 0xC) : 0xFFFFFFFFu);
        }
    }
    if (msg == 0x18C) {
        if (d2rd8(qd + 9) && d2rd8(qd + 0xB) < 5) { d2wr8(qd + 0x14, 0); d2wr8(qd + 0xB, 5); }
        if (!d2rd8(ext + 0x16) && e->call(0xA478, 0, 0, 0, 0, 0, d2rd32(arg + 0xC)) == d2rd32(ext + 8)) {
            volatile uint32_t coord[2];
            volatile uint32_t room = 0;
            e->call(0xA544, 0, 0, 0, 0, 0, d2rd32(arg + 0xC), (uint32_t)(uintptr_t)coord);
            {
                const uint32_t w[6] = { d2rd32(ext + 8), (uint32_t)(uintptr_t)coord, 2, 0xBE11, (uint32_t)(uintptr_t)&room, 0x64 };
                e->call(0x34130, 0, 0, 0, 0, 0, w[0], w[1], w[2], w[3], w[4], w[5]);
                e->blk(0x34130, w, 6);
            }
            if (room) {
                const uint32_t pl = d2rd32(arg + 0xC);
                const uint32_t qgame = d2rd32(qd + 4);
                uint32_t x = coord[0], y = coord[1];
                if (pl && e->call(0xA808, 0, 0, 0, 0, 0, room)) {
                    e->call(0xDFFD0, 0, 0, 0, 0, pl, 0x18, pl);
                } else {
                    const uint8_t act = (uint8_t)e->call(0xA56E, 0, 0, 0, 0, 0, 0x2E);
                    const uint32_t level = e->call(0xA556, 0, 0, 0, 0, 0, room);
                    if (act != (uint8_t)e->call(0xA56E, 0, 0, 0, 0, 0, level)) return;   // retail: assert 0xE5C, exit
                    volatile uint32_t pt[2];
                    pt[0] = x; pt[1] = y;
                    {
                        const uint32_t w[7] = { room, (uint32_t)(uintptr_t)pt, (uint32_t)(uintptr_t)pt, 3, 0x3E01, 0xC01, 0 };
                        const uint32_t ok = e->call(0xAE98, 0, 0, 0, 0, 0, w[0], w[1], w[2], w[3], w[4], w[5], w[6]);
                        e->blk(0xAE98, w, 7);
                        if (ok) {
                            x = pt[0]; y = pt[1];
                            const uint32_t r3 = e->call(0x1280, room);
                            if (r3) {
                                const uint32_t v[7] = { x, y, qgame, r3, 1, 1, 0 };
                                const uint32_t obj = e->call(0xE1D90, 0, 2, 0x3C, 0, 0, v[0], v[1], v[2], v[3], v[4], v[5], v[6]);
                                e->blk(0xE1D90, v, 7);
                                e->call(0xA4A2, 0, 0, 0, 0, 0, obj, 1);
                                const uint32_t portal = e->call(0xA22E0, 0, 0, 0, 0, 0, qgame, obj, 0x2E, level);
                                if (portal) {
                                    const uint8_t fl = (uint8_t)(e->call(0xAEE0, 0, 0, 0, 0, 0, portal) | 3);
                                    e->call(0xAEC8, 0, 0, 0, 0, 0, portal, (level & ~0xFFu) | fl);
                                    e->call(0xA51A, 0, 0, 0, 0, 0, r3, 0);
                                    const uint32_t pr = e->call(0xA478, 0, 0, 0, 0, 0, portal);
                                    e->call(0xA51A, 0, 0, 0, 0, 0, pr, 0);
                                    d2wr8(ext + 0x16, 1);
                                    e->call(0x33920, d2rd32(arg + 0), 0, 0, 0, 0, d2rd32(qd + 0xE0), 0xD);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (cls == 0xB1) {
        if (msg != 0x175) return;
        e->call(0x35EB0, 2, 0, 0, 0, qd);
        d2wr8(ext + 4, 1);
        e->call(0xC5D10, ext, 0, 0, 0, 0, qd);
        e->call(0x35910, d2rd32(arg + 8), d2rd32(arg + 0xC), 0, 0, 0, d2rd32(arg + 0));
        return;
    }
    if (cls == 0xC9 && msg == 0x179) {
        e->call(0x35EB0, 3, 0, 0, 0, qd);
        e->call(0x35910, d2rd32(arg + 8), d2rd32(arg + 0xC), 0, 0, 0, d2rd32(arg + 0));
        d2wr8(qd + 0x14, 0);
        d2wr8(qd + 0xB, 3);
        e->call(0xDFCC0, 0, 0, 0, mod(0xC5030), d2rd32(qd + 4), 0);
        e->call(0xDFCC0, 0, 0, 0, mod(0xC3550), d2rd32(qd + 4), 0);
    }
}
