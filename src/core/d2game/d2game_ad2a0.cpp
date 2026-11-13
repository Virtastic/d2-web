// 1.13c D2Game+0xAD2A0 -- five stack words (ret 0x14): unit, target, unused, packed (hi, lo), unused. Returns 0 or 1.
// n = 0xA802(unit, hi, lo) must be non-zero. Cap on n: target type 0 gives 0x32; type 1 gives 0x32 when 0x22A80(EAX target, 0xC), 0xA580(0, target) or 0xA7F6(target) answers,
// 0x4B when 0x22A80(EAX target, 2) answers, else 0x5A; other types return 0. A 9-word request {unit, target, 0, 1, 0x2EE, 0x43, -n, 0x18, 0} goes to 0xA00B0 (EAX);
// a non-null result gets 0xA76C(res, 0x44 and 0x45, -n, 0) and 0xA59E(target, string, 0x88B), result 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fad2a0(GE& e, uint32_t unit, uint32_t tgt, uint32_t packed)
{
    if (!unit || !tgt) return 0;
    int32_t n = (int32_t)e.h(0xA802, 0, 0, 0, 0, 0, unit, packed >> 16, packed);
    if (!n) return 0;
    const uint32_t t = e.rd32(tgt);
    int32_t cap;
    if (t == 0) cap = 0x32;
    else if (t != 1) return 0;
    else if (e.h(0x22A80, tgt, 0, 0, 0, 0, 0xC)) cap = 0x32;
    else if (e.h(0xA580, 0, 0, 0, 0, 0, 0, tgt)) cap = 0x32;
    else if (e.h(0xA7F6, 0, 0, 0, 0, 0, tgt)) cap = 0x32;
    else if (e.h(0x22A80, tgt, 0, 0, 0, 0, 2)) cap = 0x4B;
    else cap = 0x5A;
    if (n >= cap) n = cap;
    uint32_t req[9] = { unit, tgt, 0, 1, 0x2EE, 0x43, (uint32_t)-n, 0x18, 0 };
    const uint32_t r = e.h(0xA00B0, (uint32_t)(uintptr_t)req);
    e.blk(0xA00B0, (uint32_t)(uintptr_t)req, 9);
    if (!r) return 0;
    e.h(0xA76C, 0, 0, 0, 0, 0, r, 0x44, (uint32_t)-n, 0);
    e.h(0xA76C, 0, 0, 0, 0, 0, r, 0x45, (uint32_t)-n, 0);
    e.h(0xA59E, 0, 0, 0, 0, 0, tgt, 0xB20FB660u, 0x88B);
    return 1;
}
}
