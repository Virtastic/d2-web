// 1.13c D2Client+0x2E900 (632 B, stdcall ret 0x14: a1, dst, a3, item, a5): class record = *(dt+0xA78) + 0x1A8 * (int16)item[+0xBC], bounds-checked
// against dt+0xA80; C244(idx, g, cls, word, 8, &local) with g = G2 when the game mode is 2..3 else 1; A = local[5], B = local[6]; the stat-0x19 entries of
// item+0x54/+0x68 (6) and +0x98/+0xA4 (5) sum C316(a1, value, a3, a5); a nonzero sum (floored at -90) scales A and B (d2c_tip_pct); 2D220(EAX=0x10A0; dst, A, B, 0). Returns 1, 0 on a bad class.
#include "d2client_tip.h"
extern "C" uint32_t d2c_port_2e900(const D2cTipEnv& t, uint32_t a1, uint32_t dst, uint32_t a3, uint32_t item, uint32_t a5) {
    return d2c_tip_body(t, a1, dst, a3, item, a5, false);
}
