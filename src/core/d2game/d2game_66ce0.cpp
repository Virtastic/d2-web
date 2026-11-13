// 1.13c D2Game+0x66CE0 -- quest 4 message handler: message 0x94 answers 0x61 (state 2), 0x70 (trade the scroll for the key item) and 0x76 (reward: flags, the ring by difficulty, the completion); message 0x109 answers 0x7D, 0x7E and 0x7B record the player and refresh
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f66ce0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18), game = e.rd32(a), pl = e.rd32(a + 0xc);
    const uint32_t m = e.rd32(a + 0x14) & 0xffff, w = e.rd32(a + 0x18) & 0xffff;
    const uint32_t buf = e.qbuf(game, pl);
    // the vectors take only the exits below: every other one answers a word left on the stack
    if (m == 0x94) {
        if (w == 0x61) {
            e.wr8(data + 0x53, 1); e.wr8(q + 0xc, 2);
            e.say(e.rd32(q + 4), 2);
            return e.h(0x35910, e.rd32(a + 8), pl, 0, 0, 0, game);
        }
        if (w == 0x70) {
            uint32_t r = e.h(0xE660, pl, 0, 0, game, 0, 0x20736b62);
            if (!r) return 0;
            if (e.h(0xE660, pl, 0, 0, game, 0, 0x20736b62)) e.h(0x365E0, 0, pl, 0, 0, 0, game, e.rd32(pl + 0x60));
            r = e.h(0x36470, 0, 0x20646b62, pl, 0, 0, game, 2, 1);
            if (!r) { const uint32_t c = e.rd32(data + 0x7c) - 1; e.wr32(data + 0x7c, c); return c; }
            e.wr8(data + 0x64, 1); e.wr8(data + 0x4b, 1); e.wr8(data + 0x4e, 1);
            e.wr32(data + 0x38, e.rd32(r + 0xc));
            if (e.trace) e.h(0xCBE70, 0, 0, 0xB2000000u | 0xFBF18, 0, 0, e.rd32(q + 4), 4);
            r = e.h(0x35EB0, 5, 0, 0, 0, q);
            e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 3);
            return r;
        }
        if (w == 0x76) {
            uint32_t r = e.fl(buf, 4, 1);
            if (!r) return 0;
            e.setf(buf, 4, 0); e.clrf(buf, 4, 1);
            e.h(0x33880, q + 0x1c, 0, 0, 0, (pl ? e.rd32(pl + 0xc) : 0xffffffffu));
            e.h(0x34C00, pl, 0, 0, 0, 0, e.rd32(q + 4));
            const uint32_t diff = e.rd8(game + 0x6d);
            const uint32_t lvl = diff == 1 ? 0x1e : diff == 2 ? 0x3c : 7, cnt = diff == 1 || diff == 2 ? 6 : 4;
            e.h(0x36470, lvl, 0x206e6972, pl, 0, 0, game, cnt, 1);
            e.h(0x34B20, pl, 0, 0, 0, 0, 4);
            e.h(0x35910, e.rd32(a + 8), pl, 0, 0, 0, game);
            r = e.fl(buf, 4, 0xd);
            if (r) {
                if (e.rd8(q + 0xb) != 0xd) e.wr8(q + 0xb, 0xd);
                if (e.rd8(q + 0xc) != 6) e.h(0x35EB0, 6, 0, 0, 0, q);
                r = e.h(0x338D0, game, 0, 0, 0, 0, 4, 0xd);   // the vectors keep this answer set: a clear one goes on through the record's own function pointer
            }
            if (e.rd8(data + 0x63) != 1) return r;
            return e.h(0x33920, e.rd32(q + 4), 0, 0, 0, 0, 4, 0xd);
        }
    } else if (m == 0x109) {
        if (w == 0x7d) {
            e.h(0x33880, q + 0x1c, 0, 0, 0, (pl ? e.rd32(pl + 0xc) : 0xffffffffu));
            e.h(0x33830, (pl ? e.rd32(pl + 0xc) : 0xffffffffu), data + 0xb4);
            return e.h(0x35910, e.rd32(a + 8), pl, 0, 0, 0, game);
        }
        if (w == 0x7e || w == 0x7b) {
            e.h(0x33880, data + 0x138, 0, 0, 0, (pl ? e.rd32(pl + 0xc) : 0xffffffffu));
            return e.h(0x35910, e.rd32(a + 8), pl, 0, 0, 0, game);
        }
    }
    return 0;
}
}
