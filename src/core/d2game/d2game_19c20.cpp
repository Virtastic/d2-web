// src/core/d2game/d2game_19c20.cpp -- 1.13c D2Game.dll sub_19C20 written from the disassembly.
// EAX = pUnit (the acting monster), stack S0 = pGame, S1 = pointer to a pointer to the AI command record (ret 8); returns AL.
// Every callee is an explicit seam in retail register/stack order.
#include <cstdint>
#include <cstring>

struct D2Game19C20Env {
    uint32_t (*tgt)(uint32_t game, uint32_t slotp);                                       // 0xF30C0: stack (game, &slot) -> the target unit
    uint32_t (*typ)(uint32_t unit, uint32_t tgt);                                         // 0xB0A20: EBX unit, stack tgt
    uint32_t (*a6a6)(uint32_t v);                                                         // import 0xA6A6, 1 stack word
    uint32_t (*a4c0)(uint32_t tgt);                                                       // import 0xA4C0, 1 stack word -> pointer
    uint32_t (*b07b0)(uint32_t unit, uint32_t a, uint32_t b);                             // 0xB07B0: EAX unit, stack (rec+0x18, rec+0x1c)
    void (*cmd)(uint32_t game, uint32_t unit, uint32_t a, uint32_t b, uint32_t c, uint32_t d);   // 0xF2540: ECX game, EDX unit, 6 stack (0,2,a,b,1,0)
    void (*ab50)(uint32_t v);                                                             // import 0xAB50, 1 stack word
    void (*done)(uint32_t game, uint32_t unit, uint32_t v);                               // 0xF22C0: EAX game, ESI unit, stack v
    void (*ffd0)(uint32_t unit, uint32_t tgt);                                            // 0xFFFD0: ESI unit, stack (0x12, tgt)
    uint32_t (*r16610)(uint32_t unit, uint32_t game, uint32_t ppp);                       // 0x16610: EAX unit, stack (game, pp, 0x10) -> AL
    void (*d1d90)(uint32_t eax, uint32_t tgt, uint32_t game, uint32_t unit);              // 0xF1D90: EAX, ECX tgt, stack (game, unit, 2, 2)
};

static inline uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }
static inline void wr(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)p + o, &v, 4); }

// 1.13c D2Game+0x19C20
extern "C" uint32_t d2game_19c20_port(uint32_t unit, uint32_t game, uint32_t pp, const D2Game19C20Env* e)
{
    uint32_t slot = 0;
    const uint32_t tgt = e->tgt(game, 0xB3000000u);
    (void)slot;
    const uint32_t typ = e->typ(unit, tgt);
    if (!unit || rd(unit, 0) != 1) return 0;
    const uint32_t ai = rd(unit, 0x14);
    if (!ai) return 0;
    const uint32_t lst = rd(ai, 0x30);
    if (!lst) return 0;
    uint32_t bx = 0;
    for (uint32_t n = rd(lst, 0); n; n = rd(n, 8)) if (rd(n, 0) == tgt) { bx = 1; break; }
    if (tgt && rd(tgt, 0) == 0) {
        if ((uint8_t)rd(tgt, 0x6c) || e->a6a6(rd(tgt, 0x60)) || rd(e->a4c0(tgt), 0x4c) == 1) bx = 1;
    }
    uint32_t any = 0;
    for (uint32_t n = rd(lst, 0); n; n = rd(n, 8)) if (rd(n, 0)) { any = 1; break; }
    const uint32_t rec = rd(pp, 0);
    if (any || bx || (int32_t)rd(rec, 0x14) > 0) {
        if (e->b07b0(unit, rd(rec, 0x18), rd(rec, 0x1c)) > 2 && (int32_t)rd(rec, 0x14) > 0x24) {
            e->cmd(game, unit, rd(rec, 0x18), rd(rec, 0x1c), 1, 0);
        } else if ((int32_t)rd(rec, 0x14) > 0) {
            if (rd(unit, 0x2c)) e->ab50(rd(unit, 0x2c));
            e->done(game, unit, 8);
        }
        const int32_t c = (int32_t)rd(rec, 0x14);
        wr(rec, 0x14, c < 0 ? 0 : (uint32_t)(c - 1));
        return 1;
    }
    if (!tgt || tgt == unit) return 0;
    if ((int32_t)typ <= 2 || (int32_t)typ >= 0x18) {
        e->ab50(rd(unit, 0x2c));
        int32_t v = (int32_t)rd(rec, 0x18);
        if (v == 0) {
            wr(rec, 0x18, 0x3c);
            if (rd(tgt, 0) == 0) e->ffd0(unit, tgt);
        } else {
            if (v < 0) wr(rec, 0x18, 0);
            if ((int32_t)rd(rec, 0x18) > 0) wr(rec, 0x18, rd(rec, 0x18) - 1);
        }
        e->done(game, unit, 0x14);
        return 1;
    }
    if (e->r16610(unit, game, pp) & 0xff) return 1;
    wr(rd(ai, 0x2c), 0x18, 7);
    e->d1d90(typ >= 5 ? 3 : typ - 2, tgt, game, unit);
    return 1;
}
