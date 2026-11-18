// 1.13c D2Client+0xC84A0: EAX = unit (+0x2C = target id). 0x2A0B0(EAX = unit) first; a zero +0x2C is retail's fatal path. rec = 0xC2DA(id).
// rec null or dword rec[0] != 1 gives 0; 0x1510(EAX = rec[4], EDX = 8) zero gives 0; 0xC5E0(rec, 0) zero gives 0; 0x4C430(ESI = rec) nonzero gives rec, else 0.
#include <cstdint>
extern "C" uint32_t d2c_port_c84a0(uint32_t unit, void (*c2a0b0)(uint32_t), void (*fatal)(), uint32_t (*c2da)(uint32_t),
                                   uint32_t (*f1510)(uint32_t, uint32_t), uint32_t (*c5e0)(uint32_t, uint32_t), uint32_t (*f4c430)(uint32_t)) {
    c2a0b0(unit);
    uint32_t id = *(const uint32_t*)(uintptr_t)(unit + 0x2C);
    if (!id) fatal();
    uint32_t rec = c2da(id);
    if (!rec || *(const uint32_t*)(uintptr_t)rec != 1) return 0;
    if (!f1510(*(const uint32_t*)(uintptr_t)(rec + 4), 8)) return 0;
    if (!c5e0(rec, 0)) return 0;
    return f4c430(rec) ? rec : 0u;
}
