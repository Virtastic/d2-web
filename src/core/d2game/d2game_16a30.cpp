// src/core/d2game/d2game_16a30.cpp -- 1.13c D2Game.dll sub_16A30 (EAX = unit, stack game, ret 4): the monster AI
// command-queue consumer (caller 0x1DB40). Nodes of the unit's AI command list (type 4 move, 5 pulse, 7 retreat) are
// counted down; each step calls a queue/action routine. Addresses are retail 32-bit words (struct pointers are 4 bytes),
// so every field is read as uint32 and used as an address; the callees are seams in retail order.
#include <cstdint>
#include <cstring>

struct D2G16A30Env {
    uint32_t (*dist_B07B0)(uint32_t unit, uint32_t a, uint32_t b);          // EAX unit, stack a b (ret 8)
    uint32_t (*walk_F2540)(uint32_t game, uint32_t unit, uint32_t x, uint32_t y);  // ECX game, EDX unit, stack 0 2 x y 0 0
    void     (*idle_F22C0)(uint32_t game, uint32_t unit, uint32_t frames);  // EAX game, ESI unit, stack frames
    void     (*close_F1B10)(uint32_t game, uint32_t unit, uint32_t eax);    // stack game unit, EAX = (n.c & ~0xff) | n.10 low byte
    void     (*kick_F2220)(uint32_t game, uint32_t unit, uint32_t eax);     // stack game unit, EAX = n.c
    void     (*stat_B366)(uint32_t pData, uint32_t id);                     // import thunk, stack (unit[0x2c], id)
    uint32_t (*rng_1180)(uint32_t seedPtr, uint32_t limit);                 // ECX seed*, EAX limit
};
static inline uint32_t R(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline void W(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
// the circular list walk of retail: head = unit->pData->[0x28]->[0x20]; first node = head[0]
static uint32_t findNode(uint32_t unit, uint32_t type) {
    uint32_t md = R(unit, 0x14);
    if (!md) return 0;
    uint32_t a = R(md, 0x28);
    if (!a) return 0;
    uint32_t head = R(a, 0x20);
    if (!head) return 0;
    uint32_t n = R(head, 0);
    while (R(n, 8) != type && n != head) n = R(n, 0);
    return R(n, 8) == type ? n : 0;
}
static inline uint32_t state(uint32_t unit) { return R(unit, 0) == 1 && R(unit, 0x14) ? R(R(unit, 0x14), 0x2C) : 0; }
static inline void setState(uint32_t unit, uint32_t v) { if (uint32_t s = state(unit)) W(s, 0x18, v); }

// 1.13c D2Game+0x16A30
extern "C" uint32_t d2game_16a30_port(const D2G16A30Env& e, uint32_t game, uint32_t unit, uint32_t* counter) {
    if (!unit) return 0;
    if (R(unit, 0) != 1) return 0;
    if (uint32_t n = findNode(unit, 4)) if ((int32_t)R(n, 0x14) > 0) {
        if (R(n, 0xC) && R(n, 0x10) && e.dist_B07B0(unit, R(n, 0xC), R(n, 0x10)) > 3) {
            setState(unit, (*counter & 3) == 0 ? 5 : 7);
            e.walk_F2540(game, unit, R(n, 0xC), R(n, 0x10));
            W(n, 0x14, R(n, 0x14) - 1);
            ++*counter;
            return 1;
        }
        e.idle_F22C0(game, unit, R(n, 0x18));
        W(n, 0x14, R(n, 0x14) - 1);
        return 1;
    }
    if (R(unit, 0x14)) if (uint32_t n = findNode(unit, 5)) {
        int32_t v = (int32_t)R(n, 0xC);
        if (v < 1) {
            if ((int32_t)R(n, 0x14) > 0) { e.idle_F22C0(game, unit, R(n, 0x18)); W(n, 0x14, R(n, 0x14) - 1); return 1; }
        } else if (v & 1) {
            e.close_F1B10(game, unit, (R(n, 0xC) & ~0xFFu) | (R(n, 0x10) & 0xFF));
            W(n, 0xC, R(n, 0xC) - 1);
            return 1;
        } else if ((int32_t)R(n, 0x14) > 0) {
            e.idle_F22C0(game, unit, R(n, 0x18));
            W(n, 0x14, R(n, 0x14) - 1);
            W(n, 0xC, R(n, 0xC) - 1);
            return 1;
        } else W(n, 0xC, (uint32_t)v - 1);
    }
    uint32_t n = findNode(unit, 7);
    if (!n) return 0;
    uint32_t sub = R(n, 0xC);
    if (!sub) return 0;
    if (sub >= 8 && sub <= 11) {
        int32_t c = (int32_t)e.dist_B07B0(unit, R(n, 0x10), R(n, 0x14));
        if (c > 0) {
            if ((int32_t)R(n, 0x18) > 0) {
                setState(unit, 7);
                if (!e.walk_F2540(game, unit, R(n, 0x10), R(n, 0x14))) e.idle_F22C0(game, unit, 0x19);
                W(n, 0x18, R(n, 0x18) - 1);
                return 1;
            }
            if (c > 1) W(n, 0xC, 0);
        }
        uint32_t id = R(unit, 4), pd = R(unit, 0x2C);
        if (id == 0x9A) e.stat_B366(pd, 0x38);
        if (R(unit, 4) == 0x9B) e.stat_B366(R(unit, 0x2C), 0x34);
        if (R(unit, 4) == 0xB2) e.stat_B366(R(unit, 0x2C), 4);
        if (R(unit, 4) == 0x195) {
            if (R(n, 0xC) == 8) e.stat_B366(R(unit, 0x2C), 0x34);
            if (R(n, 0xC) == 9) e.stat_B366(R(unit, 0x2C), 0x30);
        }
        if (R(unit, 4) != 0x1FF || (int32_t)e.rng_1180(unit + 0x20, 100) < 5) {
            if (R(n, 0xC)) {
                if (R(unit, 0x10) != R(n, 0xC)) { e.kick_F2220(game, unit, R(n, 0xC)); W(n, 0xC, 0); return 1; }
                e.idle_F22C0(game, unit, 0x32);
                return 1;
            }
            if (R(unit, 4) == 0xB2 && (int32_t)e.rng_1180(unit + 0x20, 100) < 0x42) W(n, 0xC, 8);
            return 0;
        }
    }
    W(n, 0xC, 0);
    e.idle_F22C0(game, unit, 0x32);
    return 1;
}
