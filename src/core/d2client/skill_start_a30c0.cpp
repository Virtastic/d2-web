// D2Client rva 0xA30C0: start-skill dispatch. Callees go through the injected dispatcher in retail order, registers as retail passes them.
#include <cstdint>
#include "d2client_callee.h"

// 1.13c D2Client+0xA30C0. ECX = unit (+0 type), EDX = req (+0 a, +4 b, +8 idx, +0xC id, +0x10 c).
// tab = address of the D2Client+0xBA608 table (rows 0x200 bytes, indexed by req[2]).
extern "C" uint32_t d2_client_a30c0(const D2cSvc& v, uint32_t unit, const uint32_t* unitp, uint32_t reqaddr, const uint32_t* req, uint32_t tab) {
    if (d2c_call(v, 0xc6e8, 0, 0, 0, 0, 0, unit) != 0 && (unit == 0 || unitp[0] != 1))
        d2c_call(v, 0xa0660, 0, 0, 0, 0, 0, reqaddr);
    const uint32_t p = d2c_call(v, 0xa4e20, req[3] & 0x7f, req[3], tab + (req[2] << 9), 0, 0, req[2]);
    if (p) {
        d2c_call(v, 0xc62e, 0, 0, 0, 0, 0, unit, p);
        d2c_call(v, 0xa5fb0, unit, 0, 0, 0, 0, 0);
        return d2c_call(v, 0xa1cf0, unit, 0, 0, 0, 0, req[0], req[1], req[4]);
    }
    d2c_call(v, 0x2a750, 0, 7, unit, 0, 0, 0, 0);
    d2c_call(v, 0xc7c0, 0, 0, 0, 0, 0, 1, 2, 4, 0x40, 0xB2000000u | 0xd44b4, 0x7c2);  // Skills.cpp line 0x7c2 "StartsStartSkillID - Invalid Target"
    return 0;
}
