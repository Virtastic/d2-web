// src/core/d2game/d2game_sub_16a30.cpp -- 1.13c D2Game.dll sub_16A30 (register arg EAX=unit), from the cached decompile.
// (function list name AITHINK_Fn023_Vulture is wrong.) AI command-queue consumer, caller 0x1DB40. Callees are seams in retail order.
#include <cstdint>
#include <cstring>
typedef uint8_t* P;
static inline uint32_t r32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void w32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline P rp(const void* p, uint32_t o) { P v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
struct D2Sub16A30Env {
    int32_t (*check_B07B0)();                                  // register args
    int32_t (*walk_F2540)(void* game, P unit, uint32_t, uint32_t, uint32_t x, uint8_t y, uint32_t);
    void    (*idle_F22C0)(uint32_t frames);
    void    (*close_F1B10)(void* game, P unit);
    void    (*idle_F2220)(void* game, P unit);
    int32_t (*dist_1180)(P qword);                             // FUN_6fc21180(unit+0x20)
    void    (*stat_10425)(uint32_t a, uint32_t id);            // Ordinal_10425(unit[0xb], id)
};
static P findNode(P unit, uint32_t type) {
    if (!r32(unit, 0x14)) return nullptr;
    P md = rp(unit, 0x14); P a = rp(md, 0x28);
    if (!a) return nullptr;
    P head = rp(a, 0x20);
    if (!head) return nullptr;
    P n = rp(head, 0);
    while (r32(n, 8) != type && n != head) n = rp(n, 0);
    return r32(n, 8) == type ? n : nullptr;
}
static P sd(P unit) { return (r32(unit, 0) == 1 && r32(unit, 0x14)) ? rp(rp(unit, 0x14), 0x2C) : nullptr; }
// 1.13c D2Game+0x16A30
uint32_t d2game_sub_16a30(D2Sub16A30Env& e, void* game, P unit, int32_t& counter /* DAT_6fd31f90 */) {
    if (!unit) return 0;
    if (r32(unit, 0) == 1) {
        if (P n = findNode(unit, 4)) if ((int32_t)r32(n, 0x14) > 0) {
            if (r32(n, 0xC) && r32(n, 0x10) && (uint32_t)e.check_B07B0() > 3) {
                w32(sd(unit), 0x18, (counter % 4 == 0) ? 5 : 7);
                e.walk_F2540(game, unit, 0, 2, r32(n, 0xC), (uint8_t)r32(n, 0x10), 0);
                w32(n, 0x14, r32(n, 0x14) - 1);
                counter++;
                return 1;
            }
            e.idle_F22C0(r32(n, 0x18));
            w32(n, 0x14, r32(n, 0x14) - 1);
            return 1;
        }
        if (r32(unit, 0x14)) if (P n = findNode(unit, 5)) {
            int32_t v = (int32_t)r32(n, 0xC);
            if (v < 1) {
                if ((int32_t)r32(n, 0x14) > 0) { e.idle_F22C0(r32(n, 0x18)); w32(n, 0x14, r32(n, 0x14) - 1); return 1; }
            } else if (v & 1) {
                e.close_F1B10(game, unit); w32(n, 0xC, v - 1); return 1;
            } else if ((int32_t)r32(n, 0x14) > 0) {
                e.idle_F22C0(r32(n, 0x18)); w32(n, 0x14, r32(n, 0x14) - 1); w32(n, 0xC, v - 1); return 1;
            } else w32(n, 0xC, v - 1);
        }
    }
    if (r32(unit, 0) != 1) return 0;
    P n = findNode(unit, 7);
    if (!n) return 0;
    int32_t sub = (int32_t)r32(n, 0xC);
    if (!sub) return 0;
    if (sub >= 8 && sub <= 11) {
        int32_t c = e.check_B07B0();
        if (c > 0) {
            if ((int32_t)r32(n, 0x18) > 0) {
                w32(sd(unit), 0x18, 7);
                if (!e.walk_F2540(game, unit, 0, 2, r32(n, 0x10), (uint8_t)r32(n, 0x14), 0)) e.idle_F22C0(0x19);
                w32(n, 0x18, r32(n, 0x18) - 1);
                return 1;
            }
            if (c > 1) w32(n, 0xC, 0);
        }
        uint32_t id = r32(unit, 4), u0 = r32(unit, 0x2C);
        if (id == 0x9a) e.stat_10425(u0, 0x38);
        if (id == 0x9b) e.stat_10425(u0, 0x34);
        if (id == 0xb2) e.stat_10425(u0, 4);
        if (id == 0x195) {
            if (r32(n, 0xC) == 8) e.stat_10425(u0, 0x34);
            if (r32(n, 0xC) == 9) e.stat_10425(u0, 0x30);
        }
        if (id != 0x1ff || e.dist_1180(unit + 0x20) < 5) {
            if (r32(n, 0xC)) {
                if (r32(unit, 0x10) != r32(n, 0xC)) { e.idle_F2220(game, unit); w32(n, 0xC, 0); return 1; }
                e.idle_F22C0(0x32); return 1;
            }
            if (id == 0xb2 && e.dist_1180(unit + 0x20) < 0x42) w32(n, 0xC, 8);
            return 0;
        }
    }
    w32(n, 0xC, 0);
    e.idle_F22C0(0x32);
    return 1;
}
