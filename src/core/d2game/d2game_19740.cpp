// src/core/d2game/d2game_19740.cpp -- 1.13c D2Game.dll sub_19740 written from the disassembly.
// fastcall (ECX = pGame, EDX = pUnit), one stack word (pCtx, callee pops it). Callees are an explicit seam; each
// argument is the value the retail call site loads, in the register or stack slot named beside it.
#include <cstdint>
#include <cstring>
#include "../prng.h"

struct D2Game19740Env {
    uint32_t (*ord10331)(uint32_t unit);                                   // import, stdcall 1 word
    uint32_t (*ord10057)(uint32_t v);                                      // import, stdcall 1 word
    uint32_t (*findTarget)(uint32_t unit, uint32_t game, int32_t* dist);   // 0xB1BC0: EAX unit, ECX game, stack &dist
    void (*idle)(uint32_t game, uint32_t unit, uint32_t n);                // 0xF22C0: EAX game, ESI unit, stack n
    void (*attack)(uint32_t unit, uint32_t game, uint32_t target);         // 0xF2220: EAX 4, EDI unit, stack game, target
    void (*skill)(uint32_t unit, uint32_t game, uint32_t id, uint32_t target, uint32_t lvl);   // 0xF2380: AL lvl, ECX unit, EBX game, stack id, target, 0, 0
    void (*roam)(uint32_t unit, uint32_t game, uint32_t target);           // 0xF26B0: EAX 0, ECX unit, stack game, target, 4
    void (*walk)(uint32_t eax, uint32_t game, uint32_t unit);              // 0xF1B10: AL 5 (EAX = quotient with AL replaced), stack game, unit
};

static inline uint32_t rd32_19740(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }

// 1.13c D2Game+0x19740
extern "C" void d2game_19740_port(uint32_t game, uint32_t unit, uint32_t ctx, const D2Game19740Env* e)
{
    const uint32_t info = rd32_19740(ctx, 0x1c);              // loaded before the unit checks
    if (!unit || rd32_19740(unit, 0x10) != 1) { e->idle(game, unit, 5); return; }
    d2::Seed* seed = (d2::Seed*)(uintptr_t)(unit + 0x20);
    if (e->ord10057(e->ord10331(unit)) == 0) {
        int32_t dist = 0;
        uint32_t target = e->findTarget(unit, game, &dist);
        if (target && dist < 0x14) {
            if ((int32_t)d2::rollLimited(*seed, 100) < 0x1e) {
                if (rd32_19740(unit, 4) == 0x10f) {
                    uint16_t id; std::memcpy(&id, (const uint8_t*)(uintptr_t)info + 0x170, 2);
                    e->skill(unit, game, (uint32_t)(int32_t)(int16_t)id, target, ((const uint8_t*)(uintptr_t)info)[0x180]);
                } else e->attack(unit, game, target);
                return;
            }
            if ((int32_t)d2::rollLimited(*seed, 100) < 0x1e) { e->roam(unit, game, target); return; }
            e->idle(game, unit, 10); return;
        }
    }
    const uint32_t lo = (uint32_t)d2::roll(*seed);
    if (lo % 100 < 0x14) { e->walk(((lo / 100) & ~0xffu) | 5, game, unit); return; }
    e->idle(game, unit, 10);
}
