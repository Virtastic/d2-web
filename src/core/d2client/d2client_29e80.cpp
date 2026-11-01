// 1.13c D2Client+0x29E80: interaction legality (EAX mode, ECX unit, EDI target). Mode 6/0xD: the unit's right/left
// skill (D2Common #10828/#10507) and its record (#10858); when record+0x2E is 0x13, 0x18880(skill, target, &local)
// decides. Else a type-2 target: !#10594(unit, target) || #10324(unit, target, 0x804); otherwise
// !#11138(unit, target, #10793(target)).
#include "d2client_callee.h"
// 1.13c D2Client+0x29E80
extern "C" uint32_t d2c_29e80(const D2cSvc& v, uint32_t mode /*EAX*/, uint32_t unit /*ECX*/, const uint32_t* target /*EDI*/) {
    const uint32_t t = (uint32_t)(uintptr_t)target;
    uint32_t loc[2] = {};
    if (mode == 6 || mode == 0xd) {
        const uint32_t sk = d2c_call(v, mode == 6 ? 0xc5bc : 0xc5ce, 0, 0, 0, 0, 0, unit);
        if (sk) {
            const uint8_t* rec = (const uint8_t*)(uintptr_t)d2c_call(v, 0xc640, 0, sk, 0, 0, 0);
            if (rec && *(const uint16_t*)(rec + 0x2e) == 0x13)
                return d2c_call(v, 0x18880, 0, 0, 0, (uint32_t)(uintptr_t)&loc[1], 0, sk, t, (uint32_t)(uintptr_t)&loc[0]) != 0;
        }
    }
    if (target && target[0] == 2) {
        if (!d2c_call(v, 0xcefe, 0, 0, 0, 0, 0, unit, t)) return 1;
        return d2c_call(v, 0xcf0a, 0, 0, 0, 0, 0, unit, t, 0x804) != 0 ? 1u : 0u;
    }
    const uint32_t f = d2c_call(v, 0xcf04, 0, 0, 0, 0, 0, t);
    return d2c_call(v, 0xc6e2, 0, 0, 0, 0, 0, unit, t, f) == 0 ? 1u : 0u;
}
