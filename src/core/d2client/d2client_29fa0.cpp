// 1.13c D2Client+0x29FA0: send a one-word or two-word client packet by type (EAX type, ECX and EDX the two values).
#include <cstring>
#include "d2client_callee.h"
// 1.13c D2Client+0x29FA0
extern "C" uint32_t d2c_29fa0(const D2cSvc& v, uint32_t type, uint32_t ecx, uint32_t edx) {
    static const uint8_t kKind[17] = { 0, 1, 0, 1, 2, 1, 1, 2, 1, 1, 2, 2, 1, 1, 2, 1, 1 };
    if (type == 0x13) return 0x13;
    const uint32_t idx = type - 1;
    const uint8_t kind = idx > 0x10 ? 2 : kKind[idx];
    if (kind == 0) return d2c_call(v, 0x149e0, type, ecx, edx, 0, kind);
    uint8_t pk[12] = {};
    pk[0] = (uint8_t)type;
    uint32_t n;
    if (kind == 1) { std::memcpy(pk + 1, &ecx, 4); std::memcpy(pk + 5, &edx, 4); n = 9; }
    else { const uint16_t c = (uint16_t)ecx, d = (uint16_t)edx; std::memcpy(pk + 1, &c, 2); std::memcpy(pk + 3, &d, 2); n = 5; }
    return d2c_call(v, 0x143e0, type, 0, 0, n, 0, (uint32_t)(uintptr_t)pk);
}
