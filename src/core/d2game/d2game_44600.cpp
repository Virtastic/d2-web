// 1.13c D2Game+0x44600 -- damage-absorb check against a skill-row stat pair (stdcall, ret 0x14; words: unit, unused, damage record, skill id, unused).
// Skill row (dt +0xB98, 0x23C stride, count +0xBA0): short +0x80 selects a data record 0xA5D4(unit, id) (id < dt +0xC4); stat X = 0xA712(rec, short +0x56, 0) must be positive and
// stat Y = 0xA712(rec, short +0x54, 0) (both stat indices below dt +0xBD4). With Y non-zero the damage record's three pools (words +0x10, +0x24, +0x1C, each when positive) absorb
// it in turn (what a pool cannot cover carries on, the pool keeps the rest), Y is stored with 0xA6E2, and while it stays positive Z = 0x14D0(100, X, Y) replaces stat 0x54 (0xA706(unit, id, 1),
// 0xA6E2(rec, 0x54, Z, 0)) when it exceeds the current one by at least 5. With Y zero or used up: 0xA3CA(unit, id, 0), 0xABB6(unit, rec), 0xABAA(rec). Returns 1 once the record exists, else 0.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f44600(NE& e, uint32_t dt, uint32_t unit, uint32_t dmg, uint32_t sid)
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
    if (y != 0) {
        for (const uint32_t off : { 0x10u, 0x24u, 0x1cu }) {
            const int32_t pool = (int32_t)e.rd32(dmg + off);
            if (pool <= 0) continue;
            if (y > pool) { y -= pool; e.wr32(dmg + off, 0); }
            else { e.wr32(dmg + off, (uint32_t)(pool - y)); y = 0; }
        }
        e.imp(0xA6E2, rec, (uint32_t)k54, (uint32_t)y, 0);
        if (y > 0) {
            const int32_t z = (int32_t)e.h(0x14D0, 100, (uint32_t)x, (uint32_t)y);
            const int32_t cur = (int32_t)e.imp(0xA712, rec, 0x54, 0);
            if (z - cur >= 5) {
                e.imp(0xA706, unit, (uint32_t)s80, 1);
                e.imp(0xA6E2, rec, 0x54, (uint32_t)z, 0);
            }
            return 1;
        }
    }
    e.imp(0xA3CA, unit, (uint32_t)s80, 0);
    e.imp(0xABB6, unit, rec);
    e.imp(0xABAA, rec);
    return 1;
}
}
