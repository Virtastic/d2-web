// 1.13c D2Client+0xC63A0: ECX = unit (+0x2C = event target id), one unused stack word (ret 4). 0x2A0B0(EAX = unit) runs first; a zero +0x2C is the fatal
// path (retail logs and _exit(-1)s), reached through `fatal`. rec = 0xC2DA(unit +0x2C). A nonzero 0xC2DA record sets flag 0x40 in unit +0xC4, runs 0x4C500 (ECX = record, EAX = 0, stack 5) and returns 1; a zero record returns 0.
#include <cstdint>
extern "C" uint32_t d2c_port_c63a0(uint32_t unit, void (*c2a0b0)(uint32_t), void (*fatal)(), uint32_t (*c2da)(uint32_t), void (*c4c500)(uint32_t, uint32_t)) {
    c2a0b0(unit);
    uint32_t id = *(const uint32_t*)(uintptr_t)(unit + 0x2C);
    if (!id) fatal();
    uint32_t rec = c2da(id);
    if (!rec) return 0;
    *(uint32_t*)(uintptr_t)(unit + 0xC4) |= 0x40;
    c4c500(rec, 5);
    return 1;
}
