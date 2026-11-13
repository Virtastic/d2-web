// 1.13c D2Game+0x4DFE0 -- damage-absorb check against a skill-row stat pair, single-pool form (stdcall, ret 0x14; words: unit, unused, damage record, skill id, unused).
// Same lookups as 0x44600 (skill row short +0x80 selects the 0xA5D4 record, X = stat short +0x56 > 0, Y = stat short +0x54). When Y > 0 the damage record's word +8 absorbs it (the pool keeps
// what is left, Y keeps what the pool could not cover) and Y is stored with 0xA6E2. Y == 0 afterwards: 0xA3CA(unit, id, 0), 0xABB6(unit, rec), 0xABAA(rec). Otherwise Z = 0x14D0(100, X, Y)
// replaces stat 0x54 (0xA706(unit, id, 1), 0xA6E2(rec, 0x54, Z, 0)) when |Z - current| >= 5. Returns 1 once the record exists, else 0.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f4dfe0(NE& e, uint32_t dt, uint32_t unit, uint32_t dmg, uint32_t sid)
{
    if (!unit || !dmg) return 0;
    if ((int32_t)sid < 0 || (int32_t)sid >= (int32_t)e.rd32(dt + 0xba0)) return 0;
    const uint32_t srow = sid * 0x23cu + e.rd32(dt + 0xb98);
    if (!srow) return 0;
    const int32_t s80 = (int16_t)e.rd16(srow + 0x80);
    if (s80 < 0 || s80 >= (int32_t)e.rd32(dt + 0xc4)) return 0;
    const uint32_t rec = e.imp(0xA5D4, unit, (uint32_t)s80);
    if (!rec) return 0;
    const int32_t k56 = (int16_t)e.rd16(srow + 0x56);
    if (k56 < 0 || k56 >= (int32_t)e.rd32(dt + 0xbd4)) return 0;
    const int32_t x = (int32_t)e.imp(0xA712, rec, (uint32_t)k56, 0);
    if (x <= 0) return 0;
    const int32_t k54 = (int16_t)e.rd16(srow + 0x54);
    if (k54 < 0 || k54 >= (int32_t)e.rd32(dt + 0xbd4)) return 0;
    int32_t y = (int32_t)e.imp(0xA712, rec, (uint32_t)k54, 0);
    if (y > 0) {
        const int32_t pool = (int32_t)e.rd32(dmg + 8);
        if (y > pool) { y -= pool; e.wr32(dmg + 8, 0); }
        else { e.wr32(dmg + 8, (uint32_t)(pool - y)); y = 0; }
        e.imp(0xA6E2, rec, (uint32_t)k54, (uint32_t)y, 0);
    }
    if (y == 0) {
        e.imp(0xA3CA, unit, (uint32_t)s80, 0);
        e.imp(0xABB6, unit, rec);
        e.imp(0xABAA, rec);
        return 1;
    }
    const int32_t z = (int32_t)e.h(0x14D0, 100, (uint32_t)x, (uint32_t)y);
    const int32_t cur = (int32_t)e.imp(0xA712, rec, 0x54, 0);
    int32_t d = (int32_t)((uint32_t)z - (uint32_t)cur);
    if (d < 0) d = (int32_t)(0u - (uint32_t)d);
    if (d >= 5) {
        e.imp(0xA706, unit, (uint32_t)s80, 1);
        e.imp(0xA6E2, rec, 0x54, (uint32_t)z, 0);
    }
    return 1;
}
}
