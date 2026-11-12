// 1.13c D2Game+0x6B880: object trap callback (table entry). ecx = game, edx = the object. Spawns monster class
// 0x146 at the object when the spot is free (0x6B7A0: esi = object, stack = game, class); its result is the result.
#pragma once
#include <cstdint>

namespace g6b880 {
template <class E> uint32_t Spawn(E& e, uint32_t game, uint32_t object)
{
    return e.h(0x6B7A0, 0, game, object, 0, object, game, 0x146);
}
}
