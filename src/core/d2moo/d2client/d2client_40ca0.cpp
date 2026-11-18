// 1.13c D2Client+0x40CA0: stack = skill id, ESI = unit (may be null) (ret 4).
#include "d2c_seam.h"
extern "C" uint32_t d2c_40ca0(const D2cSkillTbl* t, const D2cSeam* s, uint8_t* unit, int32_t id) {
    const uint8_t* r = d2cSkillRec(*t, id);
    if (!r) return 0;
    const uint64_t u = (uintptr_t)unit;
    if (!(*s)(0xc1fc, 0, 0, 0, 0, 0, u, 0xc)) {
        int32_t a = d2cS16(r, 0x10c);
        if (a > 0 && a < t->nBc0) (*s)(0x6b300, 0, 0, 0, 0, 0, u, 2);
        int32_t b = d2cS16(r, 0x10e);
        if (b > 0) (*s)(0x88a70, 0, 0, 0, (uint64_t)(int64_t)b, 0, 0, 0, 0, 0);
    }
    uint64_t h = (*s)(0xc1c0, 0, 0, 0, 0, 0, u, 0xc);
    if (h) {
        (*s)(0xcb56, 0, 0, 0, 0, 0, u, (uint64_t)(int64_t)((int32_t)r[0x16] - 1));
        uint32_t v = 0x800; std::memcpy(unit + 0x48, &v, 4);
        (*s)(0xcb62, 0, 0, 0, 0, 0, h, 7);
        return 1;
    }
    uint64_t ec = 0xffffffffull, ea = 6;
    if (unit) { uint32_t c, a; std::memcpy(&c, unit + 0xc, 4); std::memcpy(&a, unit, 4); ec = c; ea = a; }
    uint64_t p = (*s)(0xc442, ea, ec, 0, 0, 0, 0, 2, 0x14, ea, ec);
    if (!p) return 0;
    uint64_t w = (*s)(0xc838, 0, u, (uint64_t)(int64_t)id);
    if (!w) return 0;
    (*s)(0xcb50, 0, 0, 0, 0, 0, w, 0);
    (*s)(0xc36a, 0, 0, 0, 0, 0, u, p, 1);
    (*s)(0xc2bc, 0, 0, 0, 0, 0, p, 0xB2000000u | 0x40190);   // the D2Client+0x40190 handler as a module tag
    (*s)(0xc2f2, 0, 0, 0, 0, 0, p, 0xc);
    (*s)(0xc30a, 0, 0, 0, 0, 0, u, 0xc, 1);
    return 1;
}
