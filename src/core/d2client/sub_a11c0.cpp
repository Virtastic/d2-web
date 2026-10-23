// D2Client+0xA11C0: register-arg predicate (EAX=skill id, EDI=u32 array); env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA11C0Env {
    void (*f2A0B0)();                    // D2Client+0x2A0B0
    void (*fail)();                      // Ordinal_10265(0x946)+10024 msg, _exit(-1); never returns
    const uint32_t* (*ord10392)(uint32_t h);  // returns u32 array (or null)
    uint32_t skillCount;                 // sgptDataTables+0xBA0
    const uint8_t* skills;               // sgptDataTables+0xB98 base, 0x23C stride
    uint32_t mask14, mask0;              // gdwBitMasks[0x14], [0]
};
extern "C" {
// 1.13c D2Client+0xA11C0
EMSCRIPTEN_KEEPALIVE int32_t d2c_a11c0(const D2cA11C0Env* e, int32_t id, const uint32_t* edi) {
    e->f2A0B0();
    if (edi[0xB] == 0) { e->fail(); return 0; }  // +0x2C
    const uint32_t* p = e->ord10392(edi[0xB]);
    if (!p) return 1;
    if (id < 0 || (uint32_t)id >= e->skillCount || !e->skills) return 0;
    const uint8_t* s = e->skills + (uint32_t)id * 0x23C;
    if (((uint8_t)e->mask14 & s[6]) && p[0] == 1 && p[4] == 0xC) return ((s[7] & (uint8_t)e->mask0) != 0);
    return 1;
}
}
