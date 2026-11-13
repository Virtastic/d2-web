// 1.13c D2Client+0xA6250 -- fastcall predicate (EDX = a unit; ECX unused): a unit passes only when the 0xC5E0 import (unit, 0) answers nonzero,
// the 0x4C430 check (ESI = the unit) answers nonzero and the unit is type 1. The skill id from 0x1700 (EAX = the unit) then decides: an id
// below 0, at or past the skill table count (the data tables word +0xA80) or other than 0x4E answers 1; the id 0x4E answers 0. Any failed
// check answers 0.
#include "cl_pa4.h"
extern "C" uint32_t d2c_a6250(uint32_t, uint32_t unit, const cl::Env& e) {
    if (!unit) return 0;
    if (!e.H(0xc5e0, unit, 0)) return 0;
    if (!e.H(0x4c430, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit)) return 0;
    if (e.R(unit) != 1) return 0;
    const int32_t id = (int32_t)e.H(0x1700, 0, 0, 0, 0, 0, unit);
    if (id < 0 || id >= (int32_t)e.R(e.dt + 0xa80) || id != 0x4e) return 1;
    return 0;
}
