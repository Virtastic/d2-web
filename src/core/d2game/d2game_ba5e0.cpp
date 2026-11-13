// 1.13c D2Game+0xBA5E0 -- npc messages of quest 0x1A part one: 0x16F/0x29E opens the quest, 0x16F/0x2A4 closes the reward, 0x196/0x2A3 moves the phase to 4
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fba5e0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t m14 = NE::rd16(a + 0x14), m18 = NE::rd16(a + 0x18), data = e.rd32(q + 0x18), game = e.rd32(q + 4);
    const uint32_t pl = e.rd32(a + 0xc), buf = e.qbuf(game, pl), fid = e.rd32(q + 0xe0);
    if (m14 == 0x16f) {
        if (m18 == 0x29e) {
            e.wr8(data + 0x11, 1); e.wr8(q + 0xc, 2);
            e.say(e.rd32(q + 4), 2);
            e.cb(e.rd32(q + 4), 0xB96A0);
            return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, e.rd32(a));
        }
        if (m18 != 0x2a4) return m18;                   // the vectors skip this exit: the high word is stack residue
        if (!e.fl(buf, fid, 1)) return 0;
        if (e.fl(buf, fid, 0xd)) {
            if (e.rd8(q + 0xc) != 5) {
                e.h(0x35EB0, 5);
                e.h(0xF8D14, 0, 0, 0, 0, 0, e.rd32(q + 0xf0));          // the code-pointer check, then the quest's own state callback with ECX = the record (not reached by the vectors)
    e.h(e.rd32(q + 0xf0) & 0xffffffu, 0, q);
                e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 0xd);
            }
            e.wr32(q + 0xa8, 0);
            e.h(0x33920, e.rd32(a), 0, 0, 0, 0, fid, 0xd);
        }
        e.setf(buf, fid, 0); e.clrf(buf, fid, 1);
        e.imp(0xA574, buf, fid);
        e.imp(0xA4D2, pl, 5, 2, 0);
        e.h(0x34B20, pl, 0, 0, 0, 0, 0x16);
        e.h(0x33880, q + 0x1c, 0, 0, 0, (pl ? e.rd32(pl + 0xc) : 0xffffffffu));
        return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, e.rd32(a));
    }
    if (m14 != 0x196 || m18 != 0x2a3) return m14;       // skipped by the vectors likewise
    e.wr8(data + 0x12, 1); e.wr8(data + 0xf, 1);
    const uint32_t r = e.setf(buf, fid, 5);
    if (e.rd8(q + 0xb) == 4) return NE::lo(r, 4);
    e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 4);
    return e.cb(e.rd32(q + 4), 0xB9D20);
}
}
