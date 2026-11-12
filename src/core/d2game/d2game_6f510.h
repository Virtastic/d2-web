// 1.13c D2Game+0x6F510: quest event callback (table entry). ecx = game, edx = the event id. Hands over to the
// quest event dispatcher (0x36C50: eax = the id, same ecx and edx); its result is the result.
#pragma once
#include <cstdint>

namespace g6f510 {
template <class E> uint32_t Dispatch(E& e, uint32_t game, uint32_t id)
{
    return e.h(0x36C50, id, game, id);
}
}
