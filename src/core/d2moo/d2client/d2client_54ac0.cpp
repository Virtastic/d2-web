// 1.13c D2Client+0x54AC0 -- does every skill row sharing the record's group word (+0x3E) still yield EXPECT? (stdcall 6:
// unit, owner, cur, expect, rec, flag*; ret 0x18). Group 0: 0. Walks the sgptDataTables skill table (+0xBCC, stride 0x144)
// from the last row (+0xBD4 - 1) down, re-checking the count each step (out of range or null row: 0). For each row in the
// group: *flag = (i == cur); v = 0xC4B4 (owner, i, 0); 0x546E0 (EAX = v, EDI = row; unit, i) != expect: 0. All matched: 1.
#include "cl_env.h"
extern "C" uint32_t d2c_546e0(uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_54ac0(uint32_t unit, uint32_t owner, uint32_t cur, uint32_t expect, uint32_t rec, uint32_t flag, const cl::Env& e) {
    uint32_t grp = e.R(rec + 0x3e) & 0xffff;
    if (!grp) return 0;
    for (int32_t i = (int32_t)e.R(e.dt + 0xbd4) - 1; i >= 0; --i) {
        if (i >= (int32_t)e.R(e.dt + 0xbd4)) return 0;
        uint32_t row = e.R(e.dt + 0xbcc) + (uint32_t)i * 0x144;
        if (!row) return 0;
        if ((e.R(row + 0x3e) & 0xffff) != grp) continue;
        e.W(flag, (uint32_t)i == cur);
        uint32_t v = e.H(0xc4b4, owner, (uint32_t)i, 0);
        if (expect != d2c_546e0(v, row, unit, (uint32_t)i, e)) return 0;
    }
    return 1;
}
