// 0x60A80 (ECX = quest, EDX = event; plain ret; reads the module table at D2Game+0x10A024, passed as `tbl`): quest 6 unit-event dispatch. The event holds game (+0),
// source unit (+8; its dword +4 is the source type, -1 if none), client unit (+0xC; its id at +0xC, -1 if none) and an argument (+0x14). Every outcome calls 0x33EB0
// (EAX = mode, ECX = quest, stack argument, source type; ret 8).
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x60A80
void d2game_60A80_q32(const Call& X, uint32_t quest, uint32_t ev, uint32_t tbl) {
    uint32_t game = rd32(ev, 0);
    uint32_t cu = rd32(ev, 0xc);
    uint32_t sk = questRec(X, game, cu);
    int32_t g0 = qGet(X, sk, 6, 0);
    int32_t gD = qGet(X, sk, 6, 0xd);
    int32_t g1 = qGet(X, sk, 6, 1);
    uint32_t src = rd32(ev, 8);
    uint32_t sid = src ? rd32(src, 4) : 0xffffffffu;
    uint32_t d = rd32(quest, 0x18);
    uint32_t cid = cu ? rd32(cu, 0xc) : 0xffffffffu;
    uint32_t arg = rd32(ev, 0x14);
    auto fire = [&](uint32_t mode, uint32_t s) { X(0x33EB0, mode, quest, 0, 0, 0, arg, s); };
    auto scan = [&](uint32_t base, uint32_t cnt) {
        for (uint32_t i = 0, n = rd16(d, cnt); i < n; i++) if (rd32(d, base + i * 4) == cid) return true;
        return false;
    };
    if (sid == 0x94) { if (scan(0x84, 0x104)) { fire(3, 0x94); return; } }
    else if (sid == 0x96) { if (scan(0x108, 0x188)) { fire(3, 0x96); return; } }
    else if (sid == 0x109) { if (scan(0, 0x80)) { fire(3, 0x109); return; } }
    if (g1 != 0) { fire((sid == 0x109 || sid == 0x94 || sid == 0x96) ? 4 : 3, sid); return; }
    for (uint32_t i = 0, n = rd16(quest, 0x9c); i < n; i++) if (rd32(quest, 0x1c + i * 4) == cid) { fire(4, sid); return; }
    uint8_t al = rd8(quest, 0xc);
    if (!(al == 1 && sid == 0x109 && g0 == 0)) {
        if (al == 0) return;
        if (g0 == 1 && gD == 0) return;
        if (al >= 4 && gD == 0) return;
        if (g0 == 1 && gD == 1) return;
        uint32_t t = rd32(tbl, al * 4u);
        if (t == 0xffffffffu || t >= 6) return;
        fire(t, sid);
        return;
    }
    fire(0, 0x109);
}
