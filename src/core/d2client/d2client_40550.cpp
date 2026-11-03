// 1.13c D2Client+0x40550: skill gated word. EAX = skill id, stack = unit (ret 4). -1 out of range; else rec+0xEA, or
// rec+0xE8+2*min(level,3) when the unit has the skill's state and its stat level is >= 2.
#include "d2c_seam.h"
extern "C" int32_t d2c_40550(const D2cSkillTbl* t, const D2cSeam* s, int32_t id, const uint8_t* unit) {
    const uint8_t* r = d2cSkillRec(*t, id);
    if (!r) return -1;
    int32_t out = d2cS16(r, 0xea);
    if (t->bit8 & r[4]) {
        int32_t st = d2cS16(r, 0x80), stat = d2cS16(r, 0x54);
        if (st >= 0 && st < t->nStates && stat >= 0 && stat < t->nStats) {
            uint64_t l = (*s)(0xc1c0, 0, 0, 0, 0, 0, (uintptr_t)unit, (uint64_t)st);
            if (l) {
                int32_t lv = (int32_t)(*s)(0xc4b4, 0, 0, 0, 0, 0, l, (uint64_t)(int64_t)stat, 0);
                if (lv > 1) { if (lv >= 3) lv = 3; out = d2cS16(r, 0xe8 + lv * 2); }
            }
        }
    }
    return out;
}
