// 1.13c D2Game+0x66960 -- quest 4 greeting chooser: picks the npc speech from the reward flags, the three player lists and the quest state
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
static const uint32_t kSpeech[6] = { 0xffffffffu, 0, 1, 2, 3, 4 };   // by quest state; state 4 has its own answer
static bool in(NE& e, uint32_t list, uint32_t cnt, uint32_t id) { const uint32_t n = e.rd16(cnt); for (uint32_t k = 0; k < n; k++) if (e.rd32(list + k * 4) == id) return true; return false; }
uint32_t f66960(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t game = e.rd32(a), pl = e.rd32(a + 0xc), a14 = e.rd32(a + 0x14);
    const uint32_t has = e.h(0xE660, pl, 0, 0, game, 0, 0x20736b62);
    const uint32_t npc = e.rd32(a + 8), id = npc ? e.rd32(npc + 4) : 0xffffffffu;
    const uint32_t buf = e.qbuf(game, pl), data = e.rd32(q + 0x18);
    auto speak = [&](uint32_t n, uint32_t who) { return e.h(0x33EB0, n, q, 0, 0, 0, a14, who); };
    if (id == 0x92) speak(9, id);
    if (e.rd8(data + 0x4f) == 1 && e.h(0xE660, pl, 0, 0, game, 0, 0x20646b62)) {
        if (e.h(0xE660, pl, 0, 0, game, 0, 0x20646b62)) e.h(0x365E0, 0, pl, 0, 0, 0, game, e.rd32(pl + 0x60));
        e.wr32(data + 0x7c, e.rd32(data + 0x7c) - 1);
    }
    const uint32_t f1 = e.fl(buf, 4, 1), fd = e.fl(buf, 4, 0xd);
    const uint32_t pid = (pl ? e.rd32(pl + 0xc) : 0xffffffffu);
    if (id == 0x109 && !in(e, data + 0x138, data + 0x1b8, pid) && fd) return speak(5, 0x109);
    if (f1) {
        if (id != 0x109) return speak(5, id);
        return speak(in(e, data + 0x138, data + 0x1b8, pid) ? 7 : 5, 0x109);
    }
    if (in(e, data + 0xb4, data + 0x134, pid)) return speak(6, id);
    if (in(e, q + 0x1c, q + 0x9c, pid) && id == 0x109 && !e.h(0x33800, 0, 0, data + 0x138, 0, pid)) return speak(5, 0x109);
    if (e.h(0x337D0, 0, 0, q, 0, pid) == 1) {
        if (e.fl(buf, 4, 0xe)) return speak(8, id);
        if (!e.fl(buf, 4, 0)) return 0;
        return speak(7, id);
    }
    uint32_t r = e.fl(buf, 4, 0xe);
    if (r) return r;
    const uint32_t st = e.rd8(q + 0xc);
    if (!st) return 0;
    r = e.fl(buf, 4, 0);
    if (r == 1) return r;
    r = e.fl(buf, 4, 0xf);
    if (r) return r;
    if (has) return speak(3, id);
    if (st == 4) return speak(2, id);
    if (st >= 6) return st;
    const uint32_t n = kSpeech[st];
    if (n == 0xffffffffu || n >= 0xb) return n;
    return speak(n, id);
}
}
