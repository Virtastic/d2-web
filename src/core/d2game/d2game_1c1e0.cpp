// src/core/d2game/d2game_1c1e0.cpp -- 1.13c D2Game.dll sub_1C1E0 (ECX = game, EDX = unit, stack ctx, ret 4): the ranged
// spread-shot AI step. ctx+0 -> ai control, +8 target unit, +0x14 distance, +0x1c -> monster params. Struct words are retail
// 32-bit pointers, read as uint32; callees are seams in retail order with their retail register/stack arguments.
#include <cstdint>
#include <cstring>

struct D2G1C1E0Env {
    void     (*skill_F2380)(uint32_t eax, uint32_t unit, uint32_t game, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);  // ECX unit, EBX game, stack s0..s3
    void     (*idle_F22C0)(uint32_t game, uint32_t unit, uint32_t frames);                                                   // EAX game, ESI unit
    uint32_t (*fieldC_A454)(uint32_t pData);                                                                                 // stack pData
    uint32_t (*field10_A3B8)(uint32_t pData);
    uint32_t (*dir_A7A8)(uint32_t unit, uint32_t x, uint32_t y);                                                            // AL result
    uint32_t (*dir8_AD1E)(uint32_t dir);                                                                                     // AL result
    void     (*aim_ACD0)(uint32_t unit, uint32_t target);
    uint32_t (*can_F3E30)(uint32_t target, uint32_t eaxY, uint32_t game, uint32_t skill, uint32_t x);                       // ECX target, EAX y, stack game skill x
    uint32_t (*c1130)(uint32_t target);                                                                                      // ECX target
    uint32_t (*c1100)(uint32_t target);
    void     (*path_AB2C)(uint32_t pData, uint32_t x, uint32_t y);
};
static inline uint32_t R(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline void W(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
static inline int32_t S16(uint32_t a, uint32_t o) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 2); return v; }
static inline uint32_t B8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }

// retail's two per-unit field reads: type 2/4/5 read pData directly, other types ask the D2Common getter (nothing when pData is null)
static uint32_t unitField(const D2G1C1E0Env& e, uint32_t u, uint32_t off, bool c) {
    uint32_t t = R(u, 0);
    if (t == 2 || (t > 3 && t <= 5)) return R(R(u, 0x2C), off);
    uint32_t p = R(u, 0x2C);
    if (!p) return 0;
    return c ? e.fieldC_A454(p) : e.field10_A3B8(p);
}
// 1.13c D2Game+0x1C1E0
// 1.13c D2Game+0x1C1E0
extern "C" void d2game_1c1e0_port(const D2G1C1E0Env& e, uint32_t game, uint32_t unit, uint32_t ctx) {
    static const int32_t kDir[64] = { 0,0,1,2,5,6,7,0, 1,1,1,2,3,6,7,0, 1,2,2,2,3,4,7,0, 1,2,3,3,3,4,5,6,
                                      1,2,3,4,4,4,5,6, 7,2,3,4,5,5,5,6, 7,0,3,4,5,6,6,6, 7,0,1,4,5,6,7,7 };
    static const int32_t kOff[8][2] = { {1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0} };
    const uint32_t ai = R(ctx, 0), m = R(ctx, 0x1C), t = R(ctx, 8);
    const int32_t dist = (int32_t)R(ctx, 0x14);
    const int32_t skill = S16(m, 0x170);
    if (skill >= 0 && R(ai, 0x14) == 0) {
        e.skill_F2380((((uint32_t)skill) & ~0xFFu) | B8(m, 0x180), unit, game, (uint32_t)skill, 0, 0, 0);
        W(ai, 0x1C, 0);
        W(ai, 0x14, R(game, 0xA8));
        return;
    }
    if ((int32_t)R(game, 0xA8) >= (int32_t)R(ai, 0x14)) {
        const uint32_t diff = B8(game, 0x6D);
        if (dist > S16(m, 0x68 + diff * 2)) {
            if ((int32_t)R(ai, 0x18) > 0) W(ai, 0x18, R(ai, 0x18) - 1);
            e.idle_F22C0(game, unit, 0xF);
            return;
        }
        const uint32_t tc = unitField(e, t, 0xC, true);
        const uint32_t tt = unitField(e, t, 0x10, false);
        const uint32_t uc = unitField(e, unit, 0xC, true);
        const uint32_t ut = unitField(e, unit, 0x10, false);
        const uint32_t dir = e.dir_A7A8(unit, tc, tt) & 0xFF;
        const int32_t rem = (int32_t)R(ai, 0x1C) % 8;
        const uint32_t d8 = e.dir8_AD1E(dir) & 0xFF;
        const int32_t idx = rem + (int32_t)(d8 & 7) * 8;
        const int32_t pick = (idx >= 0 && idx < 64) ? kDir[idx] : 0;   // idx < 0 needs a negative ai+0x1c, which only the table values 0..7 ever fill
        W(ai, 0x1C, (uint32_t)pick);
        const int32_t spread = S16(m, 0x6E + B8(game, 0x6D) * 2);
        const uint32_t x = (uint32_t)(kOff[pick][0] * spread) + uc;
        const uint32_t y = (uint32_t)(kOff[pick][1] * spread) + ut;
        e.aim_ACD0(unit, t);
        if (S16(m, 0x170) >= 0 && e.can_F3E30(t, y, game, (uint32_t)S16(m, 0x170), x)) {
            const uint32_t a = e.c1130(t);
            const uint32_t b = e.c1100(t);
            if (e.can_F3E30(t, a, game, (uint32_t)S16(m, 0x170), b)) {
                e.skill_F2380((((uint32_t)S16(m, 0x170)) & ~0xFFu) | B8(m, 0x180), unit, game, (uint32_t)S16(m, 0x170), 0, x, y);
                // the retail eax here is (m & ~0xff) | byte: the probe logs AL only (REGS l)
                e.path_AB2C(R(unit, 0x2C), x, y);
                const int32_t shots = (int32_t)R(ai, 0x18) + 1;
                W(ai, 0x18, (uint32_t)shots);
                const uint32_t d = B8(game, 0x6D);
                if (shots > S16(m, 0x5C + d * 2)) {
                    W(ai, 0x14, (uint32_t)S16(m, 0x62 + d * 2) + R(game, 0xA8));
                    W(ai, 0x18, 0);
                } else {
                    W(ai, 0x14, (uint32_t)S16(m, 0x56 + d * 2) + R(game, 0xA8));
                }
                return;
            }
        }
        if ((int32_t)R(ai, 0x18) > 0) W(ai, 0x18, R(ai, 0x18) - 1);
    }
    e.idle_F22C0(game, unit, 0xA);
}
