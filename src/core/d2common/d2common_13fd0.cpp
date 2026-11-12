// 1.13c D2Common+0x13FD0 -- flag pass (ecx = the level record, edx = index into the pair list, stack word 0 = the pair list): only for a record of type 3. Each of the fifteen six-dword rows of the table at image offset 0x95350 (wanted kind or 0 for any, two excluded kinds, the two list values, a flag) whose kind test passes for the kind at +0x1D0 and whose two values equal the list words at the index and the index + 1 ors its flag into the dword the pointer at +0x14 names.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f13fd0(GE& e, uint32_t ea, uint32_t p, uint32_t a, uint32_t c)
{
    if (e.rd32(p) != 3) return ea;
    static const uint32_t T[15][6] = { { 0, 2, 3, 1, 0, 4 }, { 0, 2, 3, 2, 3, 4 }, { 0, 3, 17, 2, 1, 8 }, { 0, 3, 17, 3, 0, 8 }, { 0, 3, 17, 1, 1, 16 }, { 0, 3, 17, 3, 3, 16 }, { 2, 0, 0, 0, 0, 8 }, { 2, 0, 0, 2, 2, 8 }, { 2, 0, 0, 3, 0, 8 }, { 2, 0, 0, 3, 2, 8 }, { 2, 0, 0, 0, 1, 1024 }, { 2, 0, 0, 1, 1, 1024 }, { 2, 0, 0, 2, 1, 512 }, { 2, 0, 0, 2, 2, 128 }, { 2, 0, 0, 3, 2, 256 } };
    const uint32_t out = e.rd32(p + 0x14), k = e.rd32(p + 0x1d0);
    for (const auto& r : T) {
        if (k != r[0] && r[0] != 0) continue;
        if (k == r[1] || k == r[2]) continue;
        if (e.rd32(c + a * 4) != r[3] || e.rd32(c + 4 + a * 4) != r[4]) continue;
        e.wr32(out, e.rd32(out) | r[5]);
    }
    return 0xB20954b8u;
}
}
