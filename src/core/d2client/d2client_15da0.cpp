// 1.13c D2Client+0x15DA0: fastcall (ECX owner, EDX table index), one stack argument (ret 4).
// owner[0xC4] |= 0x40 (owner may be 0). Row as in 0x15AB0. v = s16 row[0xEA]; if 0xD012(ecx owner) != 1 and s16 row[0xEC] >= 0, v = row[0xEC];
// v must lie in [0, dt+0xB6C). scale = 0xC316(owner, row[0x138], index, arg). 0xA1340(ebx owner, EDI = &X, stack word 0 = &Y) must succeed.
// (dx, dy) = (X - owner.x, Y - owner.y) goes through 0xA0800 (grow tiny vectors) then 0xA07C0 (shrink to |v|^2 <= 3 and rotate: (dx, dy) = (dy, -dx));
// both are pure and inlined here. Start = (X - dx * scale / 2, Y - dy * scale / 2); scale times 0xB30F0 on a 23-dword block
// {0x820, owner, owner, 0, v, 0, 0, x, y, ..., [11] index, [12] arg, [17] 0xC316(owner, row[0x13C], index, arg)}, x += dx, y += dy per iteration.
#include "cl_env.h"
static int32_t mul(int32_t a, int32_t b) { return (int32_t)((uint32_t)a * (uint32_t)b); }
static int32_t half(int32_t v) { return (v - (v >> 31)) >> 1; }  // cdq; sub eax, edx; sar eax, 1: halve toward zero
extern "C" uint32_t d2cl_15da0(uint32_t owner, int32_t index, uint32_t arg, const cl::Env& e) {
    if (owner) e.W(owner + 0xc4, e.R(owner + 0xc4) | 0x40);
    if (index < 0 || index >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    const uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)index * 0x23c;
    if (!row) return 0;
    int32_t v = e.R16s(row + 0xea);
    if ((int32_t)e.H(0xd012, 0, 0, 0, 0, 0, 0, owner) != 1) {   // thiscall: ECX owner
        const int32_t w = e.R16s(row + 0xec);
        if (w >= 0) v = w;
    }
    if (v < 0 || v >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    const int32_t sc = (int32_t)e.H(0xc316, owner, e.R(row + 0x138), (uint32_t)index, arg);
    int32_t X = 0, Y = 0;
    e.Ed((uint32_t)(uintptr_t)&X);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&Y, 0, 0, 0, 0, 0, 0, 0, owner)) return 0;
    int32_t dx = X - (int32_t)e.H(0x1630, 0, 0, 0, 0, 0, 0, owner);
    int32_t dy = Y - (int32_t)e.H(0x1660, 0, 0, 0, 0, 0, 0, owner);
    int32_t m = mul(dy, dy) + mul(dx, dx);   // 0xA0800
    if (m < 4) { dx = (int32_t)((uint32_t)dx << 2); dy = (int32_t)((uint32_t)dy << 2); m = mul(dy, dy) + mul(dx, dx); }
    if (m < 16) { dx = (int32_t)((uint32_t)dx << 1); dy = (int32_t)((uint32_t)dy << 1); }
    int32_t c = -dx, s = dy;                  // 0xA07C0
    while (mul(c, c) + mul(s, s) > 3) { s = half(s); c = half(c); }
    dx = s; dy = c;
    int32_t x = X - half(mul(dx, sc)), y = Y - half(mul(dy, sc));
    uint32_t d[23] = {0};
    d[0] = 0x820; d[1] = owner; d[2] = owner; d[4] = (uint32_t)v; d[11] = (uint32_t)index; d[12] = arg;
    d[17] = e.H(0xc316, owner, e.R(row + 0x13c), (uint32_t)index, arg);
    for (int32_t i = sc; i > 0; --i) {
        d[7] = (uint32_t)x; d[8] = (uint32_t)y;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
        x += dx; y += dy;
    }
    return 1;
}
