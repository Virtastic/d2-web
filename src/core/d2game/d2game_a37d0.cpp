// 1.13c D2Game+0xA37D0 -- the PeriodicStats aura event (D2MOO D2GAME_MONSTERS_AiFunction10_6FD13610, which the engine runs; same rule).
// ECX = game, EDX = unit, stack a3 and skill id (ret 8). dt = *sgptDataTables. The skill row (0x23C bytes at dt+0xB98, count
// dt+0xBA0) must exist with its +0x80 state short in [0, dt+0xC4), the unit must not be dead (D2Game+0xDFA90, EAX = unit), and
// aura = D2Game+0xA42A(unit, 0x97, id) (stdcall, 3 words) must be > 0: then D2Game+0xA19A0 (EAX = unit; game, id, aura, 1, 1, 0)
// and D2Game+0xA3120 (EAX = id, ECX = a3; game, unit, aura, 0). Anything else: D2Game+0x8E900 (EAX = unit, ECX = game,
// EBX = a3; 9) deletes the event. The function is void; EAX is left as the last callee returned it.
#pragma once
#include <cstdint>
namespace ga37d0 {
// 1.13c D2Game+0xA37D0
template <class E>
uint32_t Periodic(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t a3, int32_t id)
{
    if (id >= 0 && id < (int32_t)E::rd32(dt + 0xBA0)) {
        const uint32_t row = E::rd32(dt + 0xB98) + (uint32_t)id * 0x23C;
        if (row) {
            const int32_t st = E::rd16s(row + 0x80);
            if (st >= 0 && st < (int32_t)E::rd32(dt + 0xC4) && e.h(0xDFA90, unit) == 0) {
                const int32_t aura = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, unit, 0x97, (uint32_t)id);
                if (aura > 0) {
                    e.h(0xA19A0, unit, 0, 0, 0, 0, game, (uint32_t)id, (uint32_t)aura, 1, 1, 0);
                    return e.h(0xA3120, (uint32_t)id, a3, 0, 0, 0, game, unit, (uint32_t)aura, 0);
                }
            }
        }
    }
    return e.h(0x8E900, unit, game, 0, a3, 0, 9);
}
}
