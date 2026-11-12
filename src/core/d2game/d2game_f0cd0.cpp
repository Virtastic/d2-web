// 1.13c D2Game+0xF0CD0 -- EAX stale, ECX = a (EBX), EDX = unit, one stack argument other (ret 4). The same unit returns EAX. Otherwise the import 0xA3E2(unit, 0xC, 0) answering below 9 (signed) is returned; else the worker 0xF41C0 runs for (EAX = unit, EBX = a, EDI = other) and (EAX = other, EBX = a, EDI = unit), id = other +0xC (-1 for none), and 0xF0720 (EDX = id, stack: the player data +0x9C word of a type-0 unit via 0xA4C0, else 0; and 3) answers.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t ff0cd0(GE& e, uint32_t eax, uint32_t a, uint32_t unit, uint32_t other)
{
    if (other == unit) return eax;
    const uint32_t n = e.h(0xA3E2, 0, 0, 0, 0, 0, unit, 0xc, 0);
    if ((int32_t)n < 9) return n;
    e.h(0xF41C0, unit, 0, 0, a, 0); e.j(0xF41C0, other);
    e.h(0xF41C0, other, 0, 0, a, 0); e.j(0xF41C0, unit);
    const uint32_t id = other ? e.rd32(other + 0xc) : 0xffffffffu;
    if (unit && e.rd32(unit) == 0) {
        const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
        return e.h(0xF0720, 0, 0, id, 0, 0, e.rd32(pd + 0x9c), 3);
    }
    return e.h(0xF0720, 0, 0, id, 0, 0, 0, 3);
}
}
