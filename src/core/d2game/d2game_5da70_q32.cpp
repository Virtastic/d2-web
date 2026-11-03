// 1.13c D2Game+0x5DA70 (ECX = quest, EDX = event; plain ret): A5Q4 (quest 0x25/0x26) unit-event handler. Event: +0 game, +8 source unit (dword +4 = its type, -1 if none),
// +0xC player unit (its id at +0xC), +0x14 argument. Fires 0x33EB0 (EAX = mode, ECX = quest, stack: argument, type; ret 8). `tbl` = the module table at D2Game+0x10C468 (pinned).
// Delta vs the older Env port (d2game_batch_5d5c0.cpp A5Q4Msg): the 0x337D0 id comes from the player unit (event +0xC), not the source, and the table is at 0x10C468.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x5DA70
void d2game_5da70_q32(const Call& X, uint32_t quest, uint32_t ev, uint32_t tbl)
{
    const uint32_t game = rd32(quest, 4), pl = rd32(ev, 0xc);
    const uint32_t rec = X(kO10920, 0, 0, 0, 0, 0, pl);
    const uint32_t sk = rd32(rec, 0x10 + rd8(game, 0x6d) * 4);
    const uint32_t E0 = rd32(quest, 0xe0);
    const int32_t r0 = qGet(X, sk, E0, 0);
    const int32_t r13 = qGet(X, sk, E0, 0xd);
    const uint32_t src = rd32(ev, 8);
    const uint32_t type = src ? rd32(src, 4) : 0xffffffffu;
    const uint32_t d = rd32(quest, 0x18);
    const uint32_t arg = rd32(ev, 0x14);
    auto fire = [&](uint32_t mode, uint32_t t) { X(0x33EB0, mode, quest, 0, 0, 0, arg, t); };
    auto e660 = [&](uint32_t key) { return X(0xE660, pl, 0, 0, rd32(ev, 0), 0, key); };
    if (type == 0x20f) {
        if (rd8(quest, 0xc) == 6) {
            uint32_t blk[8] = { 0xc, src, 0, 0, 0, 0, 0, 0 };
            wr8((uint32_t)(uintptr_t)blk, 0x15, 0x64);
            X(0xA496, 0, 0, 0, 0, 0, src, 0);
            X(0x75E10, 0, 0, 0, 0, 0, rd32(ev, 0), (uint32_t)(uintptr_t)blk, 1);
        }
        return;
    }
    if (type == 0x201) {
        if (rd8(quest, 0xc) == 4) {
            if (e660(0x20656369)) return;
            if (rd32(d, 0xd8)) return;
            fire(3, 0x201);
            return;
        }
        if ((qGet(X, sk, E0, 0) || qGet(X, sk, E0, 0xf)) && rd8(d, 0x102) == 0 && qGet(X, sk, 0x25, 8) && !qGet(X, sk, E0, 7) &&
            !e660(0x20327274) && !qGet(X, sk, 0x25, 8))
            fire(5, 0x201);
    }
    if (qGet(X, sk, E0, 1)) {
        if (type == 0x200 || type == 0x201) if (qGet(X, sk, 0x25, type == 0x200 ? 9 : 8)) return;
        fire(5, type);
        return;
    }
    const uint32_t tid = pl ? rd32(pl, 0xc) : 0xffffffffu;
    if (X(0x337D0, 0, 0, quest, 0, tid) == 1) { fire(6, type); return; }
    if (r0 == 1) return;
    const uint8_t al = rd8(quest, 0xc);
    if (al >= 5 && r13 == 0) return;
    if (rd8(quest, 9) == 0) return;
    const uint32_t v = rd32(tbl, al * 4u);
    if (v == 0xffffffffu || v >= 10) return;
    fire(v, type);
}
