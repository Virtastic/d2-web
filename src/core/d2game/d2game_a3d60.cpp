// 1.13c D2Game+0xA3D60 -- may this skill be used by this unit? ECX = skill id, EDX = unit (or 0), EAX = flag; plain ret.
// dt = *sgptDataTables; skill rows of 0x23C bytes at dt+0xB98, count dt+0xBA0. No row: 0. Row short +0xBC == 0: 1.
// The short equal to the unit's class word ((unit ? unit+4 : -1) & 0xFFFF): 0. Then c = signed byte +0xBE: when c is in
// [0, dt+0xBF0) and the flag is set, the result is D2Game+0x96600() != c (EAX = the flag, EBX = unit ? unit+0xC : -1);
// otherwise 1. P is the address type (a retail pointer read from a table is 32 bits).
#pragma once
#include <cstdint>
namespace ga3d60 {
// 1.13c D2Game+0xA3D60
template <class E, class P>
uint32_t Allowed(E& e, P dt, int32_t id, P unit, uint32_t flag)
{
    if (id < 0 || id >= (int32_t)E::rd32(dt + 0xBA0)) return 0;
    const P row = (P)E::rd32(dt + 0xB98) + (P)id * 0x23C;
    if (!row) return 0;
    const int32_t sh = E::rd16s(row + 0xBC);
    if (sh == 0) return 1;
    const uint32_t cls = unit ? E::rd32(unit + 4) : 0xFFFFFFFFu;
    if (sh == (int32_t)(cls & 0xFFFF)) return 0;
    const int32_t c = (int8_t)E::rd8(row + 0xBE);
    if (c < 0 || c >= (int32_t)E::rd32(dt + 0xBF0) || !flag) return 1;
    const uint32_t ebx = unit ? E::rd32(unit + 0xC) : 0xFFFFFFFFu;
    return (int32_t)e.h(0x96600, flag, 0, 0, ebx) != c;
}
}
