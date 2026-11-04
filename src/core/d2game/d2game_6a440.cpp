// 1.13c D2Game+0x6A440 -- can the second unit be targeted by the first (ECX = candidate, EDX = attacker); returns 1 or 0.
// Written from the 1.13c disassembly as a rule over the seam environment. Gates in order: same unit, null, or not a monster (type 1)
// gives 0; 0xA6F4(attacker, candidate) == 1, 0xA514 or 0xA688 non-zero gives 0; mode (+0x10) not 1 or 2 gives 0; the class (+4) must
// pass 0x1560(class, 2), have a record from 0x1320 with byte +0xB set, and a 0x1540(candidate) owner that 0xA580(owner, candidate)
// and 0xA7EA(candidate) do not reject; the answer is 0x22A80(candidate, 0x1F) == 0.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f6a440(GE& e, uint32_t a, uint32_t b)
{
    if (a == b || !a || e.rd32(a) != 1) return 0;
    if (e.h(0xA6F4, 0, 0, 0, 0, 0, b, a) == 1) return 0;
    if (e.h(0xA514, 0, 0, 0, 0, 0, a)) return 0;
    if (e.h(0xA688, 0, 0, 0, 0, 0, a)) return 0;
    const uint32_t mode = e.rd32(a + 0x10);
    if (mode != 1 && mode != 2) return 0;
    const uint32_t cls = e.rd32(a + 4);
    if (!e.h(0x1560, cls, 0, 2)) return 0;
    const uint32_t rec = e.h(0x1320, cls);
    if (!rec || !e.rd8(rec + 0xB)) return 0;
    const uint32_t owner = e.h(0x1540, a);
    if (!owner) return 0;
    if (e.h(0xA580, 0, 0, 0, 0, 0, owner, a)) return 0;
    if (e.h(0xA7EA, 0, 0, 0, 0, 0, a)) return 0;
    return e.h(0x22A80, a, 0, 0, 0, 0, 0x1F) == 0 ? 1 : 0;
}
}
