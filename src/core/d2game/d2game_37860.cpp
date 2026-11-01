// 1.13c D2Game+0x37860
// Register convention: ECX = key, EDI = pGame; returns the number of counted units.
// It iterates every unit of pGame's 128 player-slot chains whose state 7 is clear (0xDFC10) and, per unit, runs the
// 0x37650 callback: if D2Common(#a556)(D2Common(#a478)(unit)) == key and the unit word at +0x10 is neither
// 0 nor 0x11, the counter (second dword of the record, the function's local that it returns) is incremented.
#include <cstdint>
#include "d2game_37860.h"

namespace {
struct Ctx { const D2G37860Env* env; uint32_t key; uint32_t count; };
void cb(void* c, void* unit)
{
    Ctx* x = static_cast<Ctx*>(c);
    uint32_t v = x->env->a556(x->env->a478(unit));
    if (v != x->key) return;
    if (unit == nullptr) return;
    uint32_t f = x->env->field10(unit);
    if (f != 0 && f != 0x11) ++x->count;
}
}

// 1.13c D2Game+0x37860
uint32_t d2g_37860(void* game, uint32_t key, const D2G37860Env* env)
{
    Ctx c{ env, key, 0 };
    env->iterate(&c, game, cb);
    return c.count;
}
