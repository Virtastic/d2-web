// 1.13c D2Client+0x40450: skill calc evaluation. EDI = skill id, EBX = unit, stack = level (ret 4). Picks the skills.txt
// calc word (rec+0x38, or +0x3C/+0x40 when the unit has the skill's state and its stat level is >= 2) and returns
// D2Common #10786(unit, calc, id, level).
#include "d2c_seam.h"
extern "C" uint32_t d2c_40450(const D2cSkillTbl* t, const D2cSeam* s, int32_t idx, const uint8_t* unit, uint32_t level) {
    const uint8_t* r = d2cSkillRec(*t, idx);
    if (!r) return 0;
    uint32_t calc;
    std::memcpy(&calc, r + 0x38, 4);
    if (t->bit8 & r[4]) {
        int32_t st = d2cS16(r, 0x80), stat = d2cS16(r, 0x54);
        if (st >= 0 && st < t->nStates && stat >= 0 && stat < t->nStats) {
            uint64_t l = (*s)(0xc1c0, 0, 0, 0, 0, 0, (uintptr_t)unit, (uint64_t)st);
            if (l) {
                int32_t lv = (int32_t)(*s)(0xc4b4, 0, 0, 0, 0, 0, l, (uint64_t)(int64_t)stat, 0);
                if (lv > 1) { if (lv >= 3) lv = 3; std::memcpy(&calc, r + 0x34 + lv * 4, 4); }
            }
        }
    }
    return (uint32_t)(*s)(0xc316, 0, 0, 0, 0, 0, (uintptr_t)unit, calc, (uint64_t)(int64_t)idx, level);
}
