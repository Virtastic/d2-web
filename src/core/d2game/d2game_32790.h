// 1.13c D2Game+0x32790: object operate callback (table entry). ecx = game, edx = the object, one stack word = the
// player (ret 4). Runs the rack drop (0x2FA90: eax = 1 for the weapon rack, ecx = game, stack = object, player).
// Result 1.
#pragma once
#include <cstdint>

namespace g32790 {
template <class E> uint32_t WeaponRack(E& e, uint32_t game, uint32_t object, uint32_t player)
{
    e.h(0x2FA90, 1, game, 0, 0, 0, object, player);
    return 1;
}
}
