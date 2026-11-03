// 0x624D0 (ECX = quest, EDX = event; plain ret, no result): runs only when the event's source unit (+8) has type (+4) 0x10A. Finds the first type-1 node of the
// quest's game (quest +4); the control's dword +8 must be nonzero (else the fatal assert path, 0x2A3C(-1)). The mode is 0x1180(EAX = 2, ECX = client seed at unit +0x20)
// when #10174 (control +0xC, 1, 0xD) is 0, the node byte +9 is set and the client's quest 1 states 0, 0xD and 1 are all 0; otherwise the unit's 64-bit LCG seed
// (+0x20/+0x24: seed = lo * 0x6AC690C5 + hi) steps and the mode is 2 + (new low % 3). Then 0x33EB0 (EAX = mode, ECX = quest, stack event arg +0x14, 0x10A).
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x624D0
void d2game_624D0_q32(const Call& X, uint32_t quest, uint32_t ev) {
    uint32_t src = rd32(ev, 8);
    if (!src || rd32(src, 4) != 0x10a) return;
    uint32_t n = findQ(rd32(quest, 4), 1);
    uint32_t ctl = rd32(rd32(ev, 0), 0x10f4);
    if (!rd32(ctl, 8)) { X(0xFFFF0001u); return; }
    uint32_t mode;
    bool roll = false;
    if (qGet(X, rd32(ctl, 0xc), 1, 0xd) == 0 && n && rd8(n, 9)) {
        uint32_t cu = rd32(ev, 0xc);
        uint32_t sk = questRec(X, rd32(ev, 0), cu);
        if (qGet(X, sk, 1, 0) == 0 && qGet(X, sk, 1, 0xd) == 0 && qGet(X, sk, 1, 1) == 0) roll = true;
        if (roll) mode = X(0x1180, 2, cu + 0x20);
    }
    if (!roll) {
        uint32_t cu = rd32(ev, 0xc);
        uint64_t m = (uint64_t)rd32(cu, 0x20) * 0x6ac690c5u + rd32(cu, 0x24);
        wr32(cu, 0x20, (uint32_t)m);
        wr32(cu, 0x24, (uint32_t)(m >> 32));
        mode = 2 + (uint32_t)m % 3;
    }
    X(0x33EB0, mode, quest, 0, 0, 0, rd32(ev, 0x14), 0x10a);
}
