// D2Game+0x67730 -- Act 1 quest 4: put Cain (class 0x92) beside the player, or open the Tristram portal.
// stack (game, player) ret 8. Quest node 4 (missing -> fatal 0x68e); nothing happens unless its byte +9 is set and
// quest data byte +0x50 != 1. Quest data +0x54 = 3, #a4a2(player, 3).
// Cain at (x+3, y+3) in the player's room via 0xEF650(ecx 0x92, edx 1; game, room, x, y, -1, 0), then once more at
// the point 0x34130 finds. Made: +0xc4 |= 0x3000000 and the player whose id is quest data +0x3c (game+0x1120 hash)
// gets 0xDFFD0(esi player; 0x30, 0). Not made: optional 0xCBE70 (when the global 0x6FD31C20 is set), quest data
// +0x74 = 0, 0xDFCC0 with callback 0x6FC805F0; then if quest data +0x74 != 0 and byte +0x66 == 0 the portal pair is
// opened (see the spec) and byte +0x51 / +0x52 / +0x62 are updated. Both paths end with the three DFCC0 callback
// passes (0x6FC848B0, 0x6FC82960 over game; 0x6FC82A80 over node+4) and node +0x14 = 0, +0xb = 6.
#include <cstdint>
#include <cstring>

struct D2Game67730Env {
    void     (*fatal)(uint32_t line);
    void     (*a4a2)(uint32_t unit, uint32_t v);
    int32_t  (*sub1100)(uint32_t unit);
    int32_t  (*sub1130)(uint32_t unit);
    uint32_t (*a478)(uint32_t unit);
    uint32_t (*subEF650)(uint32_t ecx, uint32_t edx, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6);
    void     (*sub34130)(uint32_t room, uint32_t pt, uint32_t a3, uint32_t a4, uint32_t outRoom, uint32_t a6);
    uint32_t globalCBE70;                          // *(uint32_t*)0x6FD31C20
    void     (*subCBE70)(uint32_t edx, uint32_t s1, uint32_t s2);
    uint32_t strCBE70;                             // 0x6FD1BEAC
    void     (*subDFCC0)(uint32_t eax, uint32_t ebx, uint32_t edi, uint32_t s1);
    void     (*subDFFD0)(uint32_t esi, uint32_t s1, uint32_t s2);
    uint32_t (*a808)(uint32_t room);
    uint8_t  (*a56e)(uint32_t level);
    uint32_t (*a556)(uint32_t room);
    uint32_t (*ae98)(uint32_t room, uint32_t pt, uint32_t c, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7);
    uint32_t (*sub1280)(uint32_t room, uint32_t x, uint32_t y);
    uint32_t (*subE1D90)(uint32_t ecx, uint32_t edx, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6, uint32_t s7);
    uint32_t (*subA22E0)(uint32_t game, uint32_t obj, uint32_t one, uint32_t level);
    uint8_t  (*aee0)(uint32_t obj);
    void     (*aec8)(uint32_t obj, uint32_t v);
    void     (*a51a)(uint32_t room, uint32_t v);
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
static inline uint8_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static inline void wr8(uint32_t a, uint8_t v) { *(uint8_t*)(uintptr_t)a = v; }

// 1.13c D2Game+0x67730
void D2Game_67730(uint32_t game, uint32_t player, const D2Game67730Env* env)
{
    const uint32_t list = rd32(game + 0x10f4);
    uint32_t node = list ? rd32(list) : 0;
    while (node != 0 && rd32(node) != 4) node = rd32(node + 0xf4);
    if (node == 0) { env->fatal(0x68e); return; }
    const uint32_t q = rd32(node + 0x18);
    if (rd8(node + 9) == 0 || rd8(q + 0x50) == 1) return;
    wr32(q + 0x54, 3);
    env->a4a2(player, 3);
    int32_t x = env->sub1100(player) + 3;
    int32_t y = env->sub1130(player) + 3;
    const uint32_t room = env->a478(player);
    uint32_t cain = env->subEF650(0x92, 1, game, room, (uint32_t)x, (uint32_t)y, 0xffffffffu, 0);
    int32_t pt[2];
    uint32_t outRoom = 0;            // uninitialised in retail; 0x34130 writes it
    uint32_t lvl = 0;                // the stack slot the act/level checks reuse
    if (cain == 0) {
        pt[0] = x; pt[1] = y;
        env->sub34130(room, (uint32_t)(uintptr_t)pt, 2, 0x100, (uint32_t)(uintptr_t)&outRoom, 100);
        if (outRoom != 0) cain = env->subEF650(0x92, 1, game, outRoom, (uint32_t)pt[0], (uint32_t)pt[1], 0xffffffffu, 0);
    }
    if (cain != 0) {
        wr32(cain + 0xc4, rd32(cain + 0xc4) | 0x3000000);
        const uint32_t id = rd32(q + 0x3c);
        for (uint32_t p = rd32(game + 0x1120 + (id & 0x7f) * 4); p != 0; p = rd32(p + 0xe4)) {
            if (rd32(p + 0xc) == id) { env->subDFFD0(p, 0x30, 0); break; }
        }
    } else {
        if (env->globalCBE70) env->subCBE70(env->strCBE70, game, 4);
        wr32(q + 0x74, 0);
        env->subDFCC0(0, 0x6fc805f0u, game, player);
        const uint32_t marked = rd32(q + 0x74);
        lvl = marked;
        if (marked != 0 && rd8(q + 0x66) == 0) {
            if (room == 0) { env->fatal(0xe42); return; }
            if (env->a808(room) != 0) {
                env->subDFFD0(marked, 0x18, marked);
            } else {
                const uint8_t act = env->a56e(1);
                lvl = env->a556(room);
                if (env->a56e(lvl) != act) { env->fatal(0xe5c); return; }
                x += 3; y += 3;
                pt[0] = x; pt[1] = y;
                if (env->ae98(room, (uint32_t)(uintptr_t)pt, (uint32_t)(uintptr_t)pt, 3, 0x3e01, 0xc01, 0) != 0) {
                    const uint32_t r2 = env->sub1280(room, (uint32_t)pt[0], (uint32_t)pt[1]);
                    if (r2 != 0) {
                        uint32_t obj = env->subE1D90(2, 0x3b, (uint32_t)pt[0], (uint32_t)pt[1], game, r2, 1, 1, 0);
                        env->a4a2(obj, 1);
                        obj = env->subA22E0(game, obj, 1, lvl);
                        if (obj != 0) {
                            lvl = (lvl & 0xffffff00u) | (uint8_t)(env->aee0(obj) | 3);
                            env->aec8(obj, lvl);
                            env->a51a(r2, 0);
                            env->a51a(env->a478(obj), 0);
                            wr8(q + 0x66, 1);
                        }
                    }
                }
            }
        }
        if (rd8(q + 0x51) == 0) wr8(q + 0x52, 1);
        wr8(q + 0x62, 1);
    }
    env->subDFCC0(0, 0x6fc848b0u, game, player);
    env->subDFCC0(0, 0x6fc82960u, game, player);
    wr8(node + 0x14, 0);
    wr8(node + 0xb, 6);
    env->subDFCC0(0, 0x6fc82a80u, rd32(node + 4), 0);
}
