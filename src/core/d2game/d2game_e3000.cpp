// 1.13c D2Game+0xE3000 -- ECX unused, EDX = id, three stack arguments (the second = quest-flag buffer). AL = 1 only for id 0xfd with the D2Common import 0xA568(buffer, 16, 0) answering 0; otherwise AL = 0. The upper bytes of EAX are the caller's, or the import's answer's once it ran.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe3000(GE& e, uint32_t eax, uint32_t, uint32_t id, uint32_t, uint32_t flags, uint32_t)
{
    if (id != 0xfd) return eax & 0xFFFFFF00u;
    const uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, flags, 16, 0);
    return v ? v & 0xFFFFFF00u : 1;
}
}
