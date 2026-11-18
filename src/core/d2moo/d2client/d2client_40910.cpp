// 1.13c D2Client+0x40910: ECX = unit, EDX = skill id, stack = arg (ret 4). Sets unit+0xC4 bit 0x40.
#include "d2c_seam.h"
extern "C" uint32_t d2c_40910(const D2cSkillTbl* t, const D2cSeam* s, uint8_t* unit, int32_t id, uint32_t arg) {
    if (!d2cUnitRes(*s, unit)) return 0;
    if (!d2cSkillRec(*t, id)) return 0;
    int32_t v = (int32_t)(*s)(0x40550, (uint64_t)(int64_t)id, 0, 0, 0, 0, (uintptr_t)unit);
    if (v < 0) return 0;
    (*s)(0xa1540, (uintptr_t)unit, 0, 0, 0, 0, (uint64_t)(int64_t)v, (uint64_t)(int64_t)id, arg);
    uint32_t f; std::memcpy(&f, unit + 0xc4, 4); f |= 0x40; std::memcpy(unit + 0xc4, &f, 4);
    return 1;
}
