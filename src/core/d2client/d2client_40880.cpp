// 1.13c D2Client+0x40880: ECX = unit, EDX = skill id, stack = arg (ret 4).
#include "d2c_seam.h"
extern "C" uint32_t d2c_40880(const D2cSkillTbl* t, const D2cSeam* s, const uint8_t* unit, int32_t id, uint32_t arg) {
    if (!d2cSkillRec(*t, id)) return 0;
    int32_t v = (int32_t)(*s)(0x40550, (uint64_t)(int64_t)id, 0, 0, 0, 0, (uintptr_t)unit);
    if (v < 0) return 0;
    uint32_t l0 = 0, l1 = 0;
    if (!(*s)(0xa1340, 0, 0, 0, 0, 0, (uintptr_t)&l1)) return 0;
    (*s)(0xa1540, (uintptr_t)unit, 0, 0, 0, 0, (uint64_t)(int64_t)v, (uint64_t)(int64_t)id, arg);
    (void)l0;
    return 1;
}
