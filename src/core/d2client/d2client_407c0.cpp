// 1.13c D2Client+0x407C0: ECX = unit, EDX = skill id, stack = arg (ret 4).
#include "d2c_seam.h"
extern "C" int32_t d2c_40550(const D2cSkillTbl*, const D2cSeam*, int32_t, const uint8_t*);
extern "C" uint32_t d2c_407c0(const D2cSkillTbl* t, const D2cSeam* s, const uint8_t* unit, int32_t id, uint32_t arg) {
    if (!d2cSkillRec(*t, id)) return 0;
    int32_t v = (int32_t)(*s)(0x40550, (uint64_t)(int64_t)id, 0, 0, 0, 0, (uintptr_t)unit);
    if (v < 0) return 0;
    uint64_t res = d2cUnitRes(*s, unit);
    if (!res) return 0;
    uint64_t cb = (*s)(0xcb5c, 0, 0, 0, 0, 0, (uint64_t)(int64_t)v, arg);
    (*s)(0xa1020, res, 0, 0, cb, 0, (uintptr_t)unit, (uint64_t)(int64_t)v, 0xffffffffull, (uint64_t)(int64_t)id, arg);
    return 1;
}
