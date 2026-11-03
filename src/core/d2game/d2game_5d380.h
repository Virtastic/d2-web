// 1.13c D2Game+0x5D380 ACT5Q4_TrySpawnType79Bool): EAX = unit, stack word = quest record (ret 4).
// Spawns object 0x79 beside the unit once (state byte +0x88), then marks the state spawned (+0x88 = 1, +0x87 = 0). Returns the spawn flag.
#pragma once
#include "d2game_5d1d0.h"

namespace d2g5d380 {
template <class E> bool Run(E& e, uint32_t unit, uint32_t quest, uint32_t mod)
{
    const uint32_t st = e.rd32(quest + 0x18);
    if (e.rd8(st + 0x88) != 0) return false;
    if (!d2g5d1d0::Spawn(e, unit, e.rd32(quest + 4), false, mod)) return false;
    e.wr8(st + 0x88, 1);
    e.wr8(st + 0x87, 0);
    return true;
}
}
