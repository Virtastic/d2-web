// 1.13c D2Game+0x607D0 -- ECX unused, EDX = id, three stack arguments (the second = quest-flag buffer). AL = 1 only for id 0x9b with the D2Common import 0xA568(buffer, 0, 0) answering 0; otherwise AL = 0. The upper bytes of EAX are the caller's, or the import's answer's once it ran.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f607d0(GE& e, uint32_t eax, uint32_t, uint32_t id, uint32_t, uint32_t flags, uint32_t)
{
    if (id != 0x9b) return eax & 0xFFFFFF00u;
    const uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, flags, 0, 0);
    return v ? v & 0xFFFFFF00u : 1;
}
}
