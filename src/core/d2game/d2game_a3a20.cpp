// 1.13c D2Game+0xA3A20 -- stdcall p1, idx (ret 8). dt = *sgptDataTables; rows of 0x3C bytes at dt+0xBC, count dt+0xC4
// (read once, at entry). Row idx must exist with a nonzero short at +0x1E; every other row (in order) whose +0x1E equals it
// is asked D2Game+0xA68E(p1, i) (stdcall, 2 words); the result is 1 if any answer is nonzero. The table base and the
// per-row bound are re-read from dt for each row, as retail does.
#pragma once
#include <cstdint>
namespace ga3a20 {
// 1.13c D2Game+0xA3A20
template <class E>
uint32_t SharesGroup(E& e, uint32_t dt, uint32_t p1, int32_t idx)
{
    const int32_t count = (int32_t)E::rd32(dt + 0xC4);
    if (idx < 0 || idx >= count) return 0;
    const uint32_t row = E::rd32(dt + 0xBC) + (uint32_t)idx * 0x3C;
    if (!row) return 0;
    const int32_t g = E::rd16s(row + 0x1E);
    if (!g) return 0;
    uint32_t r = 0;
    for (int32_t i = 0; i < count; i++) {
        if (i == idx) continue;
        const uint32_t ri = i < (int32_t)E::rd32(dt + 0xC4) ? E::rd32(dt + 0xBC) + (uint32_t)i * 0x3C : 0;
        if (E::rd16s(ri + 0x1E) == g && e.h(0xA68E, 0, 0, 0, 0, 0, p1, (uint32_t)i)) r = 1;
    }
    return r;
}
}
