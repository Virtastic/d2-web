// D2Game+0x7BB90 -- AITHINK_TargetCallback_FetishShaman: EDX = caster, stack = target, search record; ret 8, always returns 0. Written from the 1.13c
// disassembly; callees are the D2Seam. target: +0 type 1, +0xC4 bit 1, +0x10 mode; record: +0/+4 best (mode 0xC) and distance, +8 max distance,
// +0xC/+0x10 best and distance, +0x14 count, +0x18 mode. 0x21260 (ECX target) is asked only while record+0x18 is 0; 0x18A0's result is 0x1850's EAX.
#include "d2game_seam4e.h"
// 1.13c D2Game+0x7BB90
extern "C" uint32_t d2game_7bb90(uint32_t caster, uint32_t target, uint32_t rec, const D2Seam* e)
{
    if (caster == target || !target || d2rd32(target) != 1) return 0;
    if (!((d2rd32(target + 0xC4) >> 1) & 1)) return 0;
    if (e->call(0xAD60, 0, 0, 0, 0, 0, target)) return 0;
    if (d2rd32(rec + 0x18) == 0 && e->call(0x21260, 0, target) != caster) return 0;
    const uint32_t id = e->call(0x18A0, target);
    const uint32_t k = e->call(0x1850, id);
    if (k != 0x8D) {
        if (k != 0x18C) return 0;
        if (d2rd32(rec + 0x18) == 1) return 0;
    }
    if (e->call(0xA514, 0, 0, 0, 0, 0, target)) return 0;
    if ((int32_t)d2rd32(rec + 0x18) < 3 && e->call(0x22A80, target, 0, 0, 0, 0, 0xC)) return 0;
    const int32_t dist = (int32_t)e->call(0x1900, caster, 0, 0, target);
    if (dist > (int32_t)d2rd32(rec + 8)) return 0;
    if (d2rd32(target + 0x10) == 0xC) {
        if (dist < (int32_t)d2rd32(rec + 4)) { d2wr32(rec, target); d2wr32(rec + 4, (uint32_t)dist); }
        return 0;
    }
    d2wr32(rec + 0x14, d2rd32(rec + 0x14) + 1);
    if (dist < (int32_t)d2rd32(rec + 0x10)) { d2wr32(rec + 0xC, target); d2wr32(rec + 0x10, (uint32_t)dist); }
    return 0;
}
