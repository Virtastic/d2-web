// src/core/d2game/d2game_1b950.cpp -- 1.13c D2Game.dll sub_1B950 written from the disassembly.
// fastcall (ECX = pGame, EDX = pUnit), one stack word pp (ret 4). Callees are an explicit seam in retail register/stack order.
#include <cstdint>
#include <cstring>
#include "../prng.h"

struct D2Game1B950Env {
    uint32_t (*d15d10)(uint32_t unit, uint32_t game);                                          // 0x15D10: EAX unit, stack game -> AL
    uint32_t (*c1f40)(uint32_t game);                                                          // 0xC1F40: EAX game
    uint32_t (*tgt)(uint32_t game, uint32_t slotp);                                            // 0xF30C0: stack (game, &slot)
    uint32_t (*typ)(uint32_t unit, uint32_t tgt);                                              // 0xB0A20: EBX unit, stack tgt
    uint32_t (*c2040)(uint32_t game);                                                          // 0xC2040: EAX game
    uint32_t (*b07b0)(uint32_t unit, uint32_t a, uint32_t b);                                  // 0xB07B0: EAX unit, stack (a, b)
    void (*cmd)(uint32_t game, uint32_t unit, uint32_t a, uint32_t b);                         // 0xF2540: ECX game, EDX unit, stack (0,2,a,b,1,0)
    void (*done)(uint32_t game, uint32_t unit, uint32_t v);                                    // 0xF22C0: EAX game, ESI unit, stack v
    void (*f2870)(uint32_t tgt, uint32_t game, uint32_t unit);                                 // 0xF2870: ECX tgt, AL 2, stack (game, unit)
    void (*f1b10)(uint32_t game, uint32_t unit);                                               // 0xF1B10: AL 3, stack (game, unit)
    uint32_t (*d19c20)(uint32_t unit, uint32_t game, uint32_t pp);                             // 0x19C20: EAX unit, stack (game, pp) -> AL
    uint32_t (*c1ee0)(uint32_t game, uint32_t locp);                                           // 0xC1EE0: EAX game, ECX &loc
};

static inline uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }
static inline void wr(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)p + o, &v, 4); }

// 1.13c D2Game+0x1B950
extern "C" void d2game_1b950_port(uint32_t game, uint32_t unit, uint32_t pp, const D2Game1B950Env* e)
{
    if (e->d15d10(unit, game) & 0xff) return;
    uint32_t nd = 0;
    if (unit && rd(unit, 0) == 1 && rd(unit, 0x14)) {
        const uint32_t ai = rd(rd(unit, 0x14), 0x28);
        if (ai) {
            const uint32_t lst = rd(ai, 0x20);
            if (lst) {
                uint32_t p = rd(lst, 0);
                while (rd(p, 8) != 10 && p != lst) p = rd(p, 0);
                if (rd(p, 8) == 10) nd = p;
            }
        }
    }
    d2::Seed* seed = (d2::Seed*)(uintptr_t)(unit + 0x20);
    if (e->c1f40(game)) {
        if (nd) {
            const uint32_t tgt = e->tgt(game, 0xB3000000u);
            const int32_t typ = (int32_t)e->typ(unit, tgt);
            if (e->c2040(game) && e->b07b0(unit, rd(nd, 0xc), rd(nd, 0x10) - 3) > 2) { e->cmd(game, unit, rd(nd, 0xc), rd(nd, 0x10) - 3); return; }
            if (e->b07b0(unit, rd(nd, 0xc), rd(nd, 0x10) - 3) > 7) { e->cmd(game, unit, rd(nd, 0xc), rd(nd, 0x10) - 3); return; }
            if (!(e->d19c20(unit, game, pp) & 0xff) && tgt && tgt != unit) {
                if (rd(tgt, 0) == 0) {
                    if (typ < 0xc) {
                        if ((int32_t)((uint32_t)d2::roll(*seed) % 1000u) < 100) e->f2870(tgt, game, unit);
                        e->done(game, unit, 200);
                        return;
                    }
                    e->done(game, unit, 0x14);
                }
                if ((int32_t)((uint32_t)d2::roll(*seed) % 1000u) < 0x32) e->f1b10(game, unit);
            }
        }
        e->done(game, unit, 0x78);
        return;
    }
    uint32_t loc[2] = { rd(nd, 0xc), rd(nd, 0x10) };
    uint32_t flag = 1;
    if (e->c1ee0(game, (uint32_t)(uintptr_t)loc)) flag = 0;
    const int32_t last = (int32_t)rd(nd, 0x18), now = (int32_t)rd(game, 0xa8);
    if (!(last < now && now - last < 200)) {
        if (e->b07b0(unit, loc[0], loc[1]) > 1) { e->cmd(game, unit, loc[0], loc[1]); wr(nd, 0x18, rd(game, 0xa8)); return; }
        if (flag && (e->d19c20(unit, game, pp) & 0xff)) { wr(nd, 0x18, rd(game, 0xa8)); return; }
    }
    e->done(game, unit, 0x14);
}
