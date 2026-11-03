// 0x5FE30 (ECX = quest record, EDX = event): A5Q3 unit-event handler. Event: +0 game, +8 monster unit, +0xC client unit.
// Sets the control block (game+0x10F4) state byte +0x20 to 0x13 and runs 0xDFCC0 (EAX 0, EBX callback, EDI game, stack 0) on 0x359C0. With the quest active
// (+9): 0xA478 on the monster stores the room at data+0x8C (null: return); +0xB = 4; 0xDFCC0 on 0x58D40; with a client unit: 0xA4C0 / 0xA568 (flag 0x28) set a
// local flag when the quest flag is clear, data+0x88 := 0, four 0xDFCC0 calls (0x599E0, 0x5AB00, 0x58BA0, 0x58AE0, stack = monster), and with the flag
// data+0x88 rolls of 0x1780 (EAX hi = idx * 0x1770 + 0xBB8 clamped to 0xFFFF, EDI lo = idx * 0x1770 + 0x5DC, ECX control + 0x18) go to 0x12B90; then 0x2DE30
// (EAX game); +0xC := 5. Then the monster's two fields (types 2, 4, 5: path +0x10 and +0xC; others via 0xA3B8 and 0xA454 on +0x2C, 0 if null) go to
// 0xA0000 (ESI monster; stack game, 0, 1, 0x271, +0xC field, +0x10 field); a result goes through 0xA478 into 0xA51A (result, 0). Void.
#include "d2game_q32.h"
using namespace q32;
static uint32_t M(uint32_t rva) { return 0xB2000000u | rva; }
// 1.13c D2Game+0x5FE30
void d2game_5fe30_q32(const Call& X, uint32_t q, uint32_t ev)
{
    const uint32_t data = rd32(q, 0x18);
    const uint32_t ctl0 = rd32(rd32(q, 4), 0x10f4);
    wr8(ctl0, 0x20, 0x13);
    X(0xDFCC0, 0, 0, 0, M(0x359c0), rd32(q, 4), 0);   // EDI (the s slot) = game
    if (rd8(q, 9)) {
        bool flag = false;
        const uint32_t room = X(0xA478, 0, 0, 0, 0, 0, rd32(ev, 8));
        wr32(data, 0x8c, room);
        if (!room) return;
        wr8(q, 0xb, 4);
        X(0xDFCC0, 0, 0, 0, M(0x58d40), rd32(q, 4), 0);
        if (rd32(ev, 0xc)) {
            if (rd8(q, 9)) {
                const uint32_t sk = questRec(X, rd32(ev, 0), rd32(ev, 0xc));
                if (qGet(X, sk, 0x28, 0) == 0) flag = true;
            }
            wr32(data, 0x88, 0);
            X(0xDFCC0, 0, 0, 0, M(0x599e0), rd32(ev, 0), rd32(ev, 8));
            X(0xDFCC0, 0, 0, 0, M(0x5ab00), rd32(ev, 0), rd32(ev, 8));
            X(0xDFCC0, 0, 0, 0, M(0x58ba0), rd32(ev, 0), rd32(ev, 8));
            X(0xDFCC0, 0, 0, 0, M(0x58ae0), rd32(ev, 0), rd32(ev, 8));
            if (flag) {
                const uint32_t idx = rd8(rd32(q, 4), 0x6d), base = idx * 0x1770u;
                const uint32_t lo = base + 0x5dc;
                int32_t hi = (int32_t)(base + 0xbb8);
                if (hi >= 0xffff) hi = 0xffff;
                if ((int32_t)rd32(data, 0x88) > 0) {
                    int32_t i = 0;
                    do {
                        const uint32_t ctl = rd32(rd32(q, 4), 0x10f4);
                        if (!ctl) {   // fatal: 0xA2DA(0x1FB), 0xA2C2(string, result), 0x2A3C(-1)
                            const uint32_t v = X(0xA2DA, 0, 0, 0, 0, 0, 0x1fb);
                            X(0xA2C2, 0, 0, 0, 0, 0, M(0xF9FF9), v);
                            X(0x2A3C, 0, 0, 0, 0, 0, 0xffffffffu);
                            return;
                        }
                        const uint32_t roll = X(0x1780, (uint32_t)hi, ctl + 0x18, 0, 0, lo);   // EDI (the s slot) = lo
                        X(0x12B90, 0, 0, 0, 0, 0, rd32(q, 4), rd32(ev, 8), roll);
                        i++;
                    } while (i < (int32_t)rd32(data, 0x88));
                }
            }
            X(0x2DE30, rd32(q, 4));
        }
        if (rd8(q, 0xc) != 5) wr8(q, 0xc, 5);
    }
    auto pathType = [](int32_t t) { return t == 2 || (t > 3 && t <= 5); };
    uint32_t mon = rd32(ev, 8), edi, eax;
    const int32_t t0 = (int32_t)rd32(mon, 0);
    if (pathType(t0)) edi = rd32(rd32(mon, 0x2c), 0x10);
    else { const uint32_t p = rd32(mon, 0x2c); edi = p ? X(0xA3B8, 0, 0, 0, 0, 0, p) : 0; }
    mon = rd32(ev, 8);
    const int32_t t1 = (int32_t)rd32(mon, 0);
    if (pathType(t1)) eax = rd32(rd32(mon, 0x2c), 0xc);
    else { const uint32_t p = rd32(mon, 0x2c); eax = p ? X(0xA454, 0, 0, 0, 0, 0, p) : 0; }
    (void)edi; (void)eax;   // stack words 4 and 5 of 0xA0000; the Call seam carries four stack words
    const uint32_t res = X(0xA0000, 0, 0, 0, 0, mon, rd32(ev, 0), 0, 1, 0x271);
    if (res) {
        const uint32_t r2 = X(0xA478, 0, 0, 0, 0, 0, res);
        X(0xA51A, 0, 0, 0, 0, 0, r2, 0);
    }
}
