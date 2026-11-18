// 1.13c D2Client+0xBBB40 (ecx = unit): missile row lookup; count = 0xC44E(unit) or the row word +0x1E (negative: stop), level = 0xC412(unit) (<= 0: stop).
// While 0xC4D2(unit) is below row +0x58 and divisible by 3, 0xBB210(level, count, 0xC388, 0xC3FA, level or level/2 above 4, 1 or 2 above 4). Tails to 0xB17C0; the stop paths run 0xB1720.
#include "cl_env.h"
extern "C" uint32_t d2c_bbb40(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row) return e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u);   // the stop paths return the callee's EAX
    int32_t cnt = (int32_t)e.H(0xc44e, u);
    if (!cnt) cnt = e.R16s(row + 0x1e);
    if (cnt < 0) return e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u);
    int32_t lv = (int32_t)e.H(0xc412, u);
    if (lv <= 0) return e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u);
    if ((int32_t)e.H(0xc4d2, u) < (int32_t)e.R(row + 0x58) && (int32_t)e.H(0xc4d2, u) % 3 == 0) {
        uint32_t w4 = lv > 4 ? (uint32_t)(lv / 2) : (uint32_t)lv;
        uint32_t w[2] = { w4, (uint32_t)(lv > 4) + 1u };
        uint32_t y = e.H(0xc3fa, u), x = e.H(0xc388, u);
        e.H(0xbb210, (uint32_t)lv, (uint32_t)cnt, x, y, w[0], u);
        e.Xw(0xbb210, w, 2);
    }
    return e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
