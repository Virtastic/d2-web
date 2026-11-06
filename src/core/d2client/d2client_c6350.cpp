// 1.13c D2Client+0xC6350: ECX = unit (+0x2C = event target id), one unused stack word (ret 4). 0x2A0B0(EAX = unit) runs first; a zero +0x2C is the fatal
// path (retail logs and _exit(-1)s), reached through `fatal`. rec = 0xC2DA(unit +0x2C). A zero 0xC2DA record returns 0; otherwise 1 when 0xC5D4(record) is nonzero.
#include <cstdint>
extern "C" uint32_t d2c_port_c6350(uint32_t unit, void (*c2a0b0)(uint32_t), void (*fatal)(), uint32_t (*c2da)(uint32_t), uint32_t (*third)(uint32_t)) {
    c2a0b0(unit);
    uint32_t id = *(const uint32_t*)(uintptr_t)(unit + 0x2C);
    if (!id) fatal();
    uint32_t rec = c2da(id);
    if (!rec) return 0;
    return third(rec) ? 1u : 0u;
}
