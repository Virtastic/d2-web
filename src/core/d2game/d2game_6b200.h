// 1.13c D2Game+0x6B200 (D2GAME_6B200_ObjectStepExtras): ESI = pUnit, [esp+4] = p (ret 4), EBP = p inside. Written from the disassembly as a rule
// over an environment E (hookenv.h): each callee is one E::h(rva, ...) call. Unit non-null: flags unit+0xC4 bit0 calls 0x6A0D0 (EAX = unit, EDI = p);
// bit10 calls 0x7AC50 (EAX = p, EDI = unit); bit8 calls 0x7ACB0 (EAX = unit, one stack word p); unit+0xC8 bit0 calls 0x9BB10 (ECX = 0, stack unit, p).
// Always: 0x7ADF0 (stack unit, p; ECX residue is not logged).
#pragma once
#include <cstdint>

namespace d2g6b200 {
template <class E> void Run(E& e, uint32_t unit, uint32_t p)
{
    if (unit) {
        const uint32_t f = e.rd32(unit + 0xC4);
        if (f & 1) { e.h(0x6A0D0, unit); e.edi(0x6A0D0, p); }
        if (f >> 10 & 1) { e.h(0x7AC50, p); e.edi(0x7AC50, unit); }
        if (f >> 8 & 1) e.h(0x7ACB0, unit, 0, 0, 0, 0, p);
        if (e.rd32(unit + 0xC8) & 1) e.h(0x9BB10, 0, 0, 0, 0, 0, unit, p);
    }
    e.h(0x7ADF0, 0, 0, 0, 0, 0, unit, p);
}
}  // namespace d2g6b200
