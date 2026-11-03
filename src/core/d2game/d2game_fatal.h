// The D2Game fatal-error tail shared by the quest/object rules: Fog #10265 formats the line number, Fog #10024 logs it under the
// 0xF9FF9 message, then the process exits. Callees are seams on the environment E (HookEnv or the engine's call table).
#pragma once
#include <cstdint>

namespace d2gfatal {
template <class E> void Fatal(E& e, uint32_t line, uint32_t mod)
{
    const uint32_t s = e.h(0xA2DA, 0, 0, 0, 0, 0, line);
    e.h(0xA2C2, 0, 0, 0, 0, 0, mod + 0xF9FF9u, s);
    e.h(0x2A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);   // _exit(-1): does not return in retail
}
}
