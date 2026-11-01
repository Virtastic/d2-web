// 1.13c D2Client+0x179E0 (with its 0x17B0D tail): monster approach step: EAX = actor (EDI), ECX = unit (ESI), stack s0 = range, ret 4
#include "cl17.h"
extern "C" uint32_t d2c_17500(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_173e0(uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_176a0(uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" uint32_t d2c_179e0(uint32_t actor, uint32_t unit, uint32_t s0, const cl::Env& e) {
    if (!unit) return actor;
    uint32_t mode = e.R(unit + 0x10), eax = actor;
    if (mode == 0xc || mode == 0) return eax;
    if (s0 < 2) {
        eax = e.H(0x16cb0, 0, 0, 0, 0, 0, unit);
        if (eax) return e.H(0x17300, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit);
    }
    if (mode != 1) return eax;
    if (s0 < 4) return d2c_17500(actor, unit, 6, mode, 0, e);
    uint32_t tgt = 0, l10 = 0, l18 = 0;
    if (s0 > 7) {
        e.H(0x16c20, cl17::ptr32(&l18), cl17::ptr32(&l10), 0, 0, 0, 0, 0, 0, 0, unit);
        tgt = l18;
    }
    uint32_t f = cl17::monField34(e, unit);
    if (!f) { cl17::setState(e, unit, 0x20); return e.H(0x17270, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit); }
    bool roll = true;                       // 0x17AB4: the 20% release roll
    if (tgt && s0 > 7) {
        if (e.R(f) == 0) { uint32_t r = d2c_176a0(tgt, unit, 0, 6, e); e.W(f, 1); return r; }
    } else if (e.R(f) == 0) {
        // 0x17B0D: 25% chance to idle (state 0x1E), else a short wander
        if ((int32_t)e.H(0x1330, 0, 0, 0, 0, 0, 100, unit + 0x20) < 0x4b) {
            cl17::setState(e, unit, 0x1e);
            return e.H(0x17270, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit);
        }
        return d2c_173e0(unit, 5, 1, 0, e);
    }
    (void)roll;
    if ((int32_t)e.H(0x1330, 0, 0, 0, 0, 0, 100, unit + 0x20) < 0x14) e.W(f, 0);
    cl17::setState(e, unit, 0x20);
    return e.H(0x17270, 0, 0, 0, 0, 0, 0, 0, 0, 0, unit);
}
