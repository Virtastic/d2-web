// Create the object a spawn record names: EBX = unit, ESI = record (+0 descriptor pointer, +8 result, +0x14 / +0x18 out slots),
// stack p; ret 4. The descriptor's first dword is read (and unused). The unit's kind record (ECX = unit) picks the path by its dword 0:
// kind 0 answers 0, kind 3 and anything above 5 answer 1. Kind 1 creates with ECX = unit, EDX = descriptor, EAX = +0x18 slot, stack
// p, +0x14 slot. Kind 2 creates with EAX = p, ECX = +0x14 slot, EDX = descriptor, stack unit, descriptor, +0x14 slot, +0x18 slot.
// Kinds 4 and 5 create with EAX = +0x18 slot, ECX = +0x14 slot, EDX = descriptor, stack p, unit. The created object goes to +8;
// kinds 1 and 4 answer 1 when nothing was made, kinds 2 and 5 always 0. A rule over E (E::h = the callee call, E::rd32/wr32 = memory).
#ifndef D2GAME_RULE_9D160
#define D2GAME_RULE_9D160
#include <cstdint>

namespace g9d160 {
template <class E> uint32_t Create(E& e, uint32_t unit, uint32_t rec, uint32_t p)
{
    const uint32_t desc = e.rd32(rec);
    (void)e.rd32(desc);
    const uint32_t kind = e.rd32(e.h(0x9D0F0, 0, unit));
    uint32_t o;
    switch (kind) {
    case 0: return 0;
    case 1: o = e.h(0xF3220, rec + 0x18, unit, desc, 0, 0, p, rec + 0x14); e.wr32(rec + 8, o); return o == 0;
    case 2: o = e.h(0xB1C90, p, rec + 0x14, desc, 0, 0, unit, desc, rec + 0x14, rec + 0x18); e.wr32(rec + 8, o); return 0;
    case 4: o = e.h(0xF3150, rec + 0x18, rec + 0x14, desc, 0, 0, p, unit); e.wr32(rec + 8, o); return o == 0;
    case 5: o = e.h(0xF3150, rec + 0x18, rec + 0x14, desc, 0, 0, p, unit); e.wr32(rec + 8, o); return 0;
    default: return 1;
    }
}
}
#endif
