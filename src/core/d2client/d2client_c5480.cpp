// 1.13c D2Client+0xC5480: fastcall(ECX = unit, EDX = skill index) + 1 stack word a3 (ret 4).
// Skill row (sgptDataTables+0xB98, stride 0x23C, count +0xBA0) missing returns 0. mi = #40550(EAX = index, stack unit) outside [0, count at +0xB6C) returns 0.
// #A1340(EBX = unit, EDI = &X, stack &Y) == 0 returns 0. cnt = #C316(unit, row+0x138, index, a3); dx = X - #1630(unit), dy = Y - #1660(unit).
// #A0800 (ESI = &dx, EDX = &dy): if dx*dx + dy*dy < 4 both are multiplied by 4 and the sum recomputed; if the sum is then < 16 both are doubled.
// #A07C0 (EBX = &dx, EDI = &dy): c = -dx, s = dy; while c*c + s*s > 3 both are halved toward zero; then dx = s, dy = c. posX = X - (dx*cnt)/2, posY = Y - (dy*cnt)/2
// (signed, truncating). Request block (23 dwords, zeroed): [0] = 0x20, [1] = [2] = unit, [4] = mi, [11] = index, [12] = a3, [19] = #C316(unit, row+0x13C, index, a3).
// cnt times (cnt <= 0 skips): [7] = posX, [8] = posY; p = #B30F0(ECX = block); if p: v = #C430(p), #73BC0(ESI = p, EDI = v); posX += dx, posY += dy.
// Then #C1FC(unit, 0xC): if nonzero, #CB56(unit, 8) and unit word +0x48 = 0x600. Returns 1.
#include "cl_env.h"
static int32_t half5480(int32_t v) { return (int32_t)((uint32_t)v - (uint32_t)(v >> 31)) >> 1; }
static int32_t sq5480(int32_t a, int32_t b) { return (int32_t)((uint32_t)a * (uint32_t)a + (uint32_t)b * (uint32_t)b); }
static void a0800_c5480(int32_t& dx, int32_t& dy) {
    int32_t sum = sq5480(dx, dy);
    if (sum < 4) {
        dx = (int32_t)((uint32_t)dx << 2);
        dy = (int32_t)((uint32_t)dy << 2);
        sum = sq5480(dx, dy);
    }
    if (sum < 0x10) {
        dx = (int32_t)((uint32_t)dx << 1);
        dy = (int32_t)((uint32_t)dy << 1);
    }
}
static void a07c0_c5480(int32_t& dx, int32_t& dy) {
    int32_t c = (int32_t)(0u - (uint32_t)dx), s = dy;
    if (sq5480(c, s) > 3) {
        do {
            s = half5480(s);
            c = half5480(c);
        } while (sq5480(c, s) > 3);
    }
    dx = s;
    dy = c;
}
extern "C" uint32_t d2c_c5480(uint32_t unit, int32_t cls, uint32_t a3, const cl::Env& e) {
    if (cls < 0 || cls >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)cls * 0x23c;
    if (!row) return 0;
    int32_t mi = (int32_t)e.H(0x40550, unit, 0, 0, 0, 0, (uint32_t)cls);
    if (mi < 0 || mi >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    uint32_t xy[2] = {};   // [0] = X (EDI), [1] = Y (stack word 0)
    e.Ed((uint32_t)(uintptr_t)&xy[0]);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&xy[1], 0, 0, 0, 0, 0, 0, 0, unit)) return 0;
    int32_t cnt = (int32_t)e.H(0xc316, unit, e.R(row + 0x138), (uint32_t)cls, a3);
    int32_t dx = (int32_t)(xy[0] - e.H(0x1630, 0, 0, 0, 0, 0, 0, unit));
    int32_t dy = (int32_t)(xy[1] - e.H(0x1660, 0, 0, 0, 0, 0, 0, unit));
    a0800_c5480(dx, dy);
    a07c0_c5480(dx, dy);
    int32_t posX = (int32_t)xy[0] - half5480((int32_t)((uint32_t)dx * (uint32_t)cnt));
    int32_t posY = (int32_t)xy[1] - half5480((int32_t)((uint32_t)dy * (uint32_t)cnt));
    uint32_t q[23] = {};
    q[0] = 0x20; q[1] = unit; q[2] = unit; q[4] = (uint32_t)mi; q[11] = (uint32_t)cls; q[12] = a3;
    q[19] = e.H(0xc316, unit, e.R(row + 0x13c), (uint32_t)cls, a3);
    for (int32_t i = cnt; i > 0; i--) {
        q[7] = (uint32_t)posX; q[8] = (uint32_t)posY;
        uint32_t p = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (p) {
            uint32_t v = e.H(0xc430, p);
            e.H(0x73bc0, 0, 0, 0, 0, 0, 0, 0, 0, 0, p);
            e.Ej(0x73bc0, v);
        }
        posY = (int32_t)((uint32_t)posY + (uint32_t)dy);
        posX = (int32_t)((uint32_t)posX + (uint32_t)dx);
    }
    if (e.H(0xc1fc, unit, 0xc)) {
        e.H(0xcb56, unit, 8);
        e.W(unit + 0x48, 0x600);
    }
    return 1;
}
