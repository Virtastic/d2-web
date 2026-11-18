// 1.13c D2Client+0xb8bc0 -- client missile: homing step and trail. ECX = unit, no return.
// No row (class +4 against sgptDataTables +0xB6C), no owner (0xA60D0, EAX = u) or count = 0xC322(u) <= -row +0x5C:
// 0xB1720(ESI = u). count <= 0: 0xC448(u, count - 1). 0xC352-style live check 0xC496(path) == 0: 0xB17C0 tail only.
// Target = 0xC2DA(path), else 0xA5F70(ESI = owner). period = row +0x60 (<= 0: 5). With a target, 0xC406(u) % period == 0,
// count > 0 and 0xC190(u, target) < row +0x64: step = max(row +0x68, 1) toward the target per axis (target x/y vs unit x/y
// from 0x1630 / 0x1660), then 0xC3EE(path, 0x1630(u) + dx, 0x1660(u) + dy) (y read first) and 0xC376(path, u, 0).
// Trail: row +0x1E > 0, 0xC406(u) % max(row +0x58, 1) == 0 and count > 0: request word 0 = 5, 1 = owner, 4 = row +0x1E,
// 5/6 = unit x/y, 10 = 0, 11/12 = 0xC388 / 0xC3FA(u), 19 = 0xC430(u) - 1; a created missile gets 0xC448(m, 0).
// Tail 0xB17C0(ECX = u).
#include "cl_env.h"
extern "C" void d2c_b8bc0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u), own = 0;
    int32_t cnt = 0;
    bool ok = row && (own = e.H(0xa60d0, 0, 0, 0, 0, 0, u)) != 0;
    if (ok) { cnt = (int32_t)e.H(0xc322, u); ok = cnt > -(int32_t)e.R(row + 0x5c); }
    if (!ok) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    if (cnt <= 0) e.H(0xc448, u, (uint32_t)(cnt - 1));
    if (!e.H(0xc496, e.R(u + 0x2c))) { e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t tg = e.H(0xc2da, e.R(u + 0x2c));
    if (!tg) tg = e.H(0xa5f70, 0, 0, 0, 0, 0, 0, 0, 0, 0, own);
    int32_t per = (int32_t)e.R(row + 0x60);
    if (per <= 0) per = 5;
    if (tg && (int32_t)e.H(0xc406, u) % per == 0 && cnt > 0 && (int32_t)e.H(0xc190, u, tg) < (int32_t)e.R(row + 0x64)) {
        int32_t ux = (int32_t)e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        int32_t uy = (int32_t)e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        int32_t tx = (int32_t)e.H(0x1630, 0, 0, 0, 0, 0, 0, tg);
        int32_t ty = (int32_t)e.H(0x1660, 0, 0, 0, 0, 0, 0, tg);
        int32_t st = (int32_t)e.R(row + 0x68);
        if (st < 1) st = 1;
        int32_t dx = tx > ux ? st : tx < ux ? -st : 0;
        int32_t dy = ty > uy ? st : ty < uy ? -st : 0;
        uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u) + (uint32_t)dy;
        uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u) + (uint32_t)dx;
        e.H(0xc3ee, e.R(u + 0x2c), x, y);
        e.H(0xc376, e.R(u + 0x2c), u, 0);
    }
    if (e.R16s(row + 0x1e) > 0) {
        int32_t p2 = (int32_t)e.R(row + 0x58);
        if (p2 <= 1) p2 = 1;
        if ((int32_t)e.H(0xc406, u) % p2 == 0 && cnt > 0) {
            uint32_t q[23] = {};
            q[0] = 5;
            q[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
            q[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
            q[19] = e.H(0xc430, u) - 1;
            q[10] = 0;
            q[4] = (uint32_t)e.R16s(row + 0x1e);
            q[1] = own;
            q[11] = e.H(0xc388, u);
            q[12] = e.H(0xc3fa, u);
            uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
            if (m) e.H(0xc448, m, 0);
        }
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
