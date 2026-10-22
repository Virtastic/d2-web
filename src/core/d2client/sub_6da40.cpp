// D2Client+0x6DA40: register-arg predicate (EAX=idx, ESI=const u32[2]{type,id} or null); env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2c6DA40Env {
    int32_t (*ord10175)(const uint32_t* esi);   // Ordinal_10175 -> switch selector
    const uint8_t* (*f1260)(uint32_t id);       // D2Client+0x1260 (bytes at +0xe8)
    const uint8_t* (*ord10688)(uint32_t id);    // Ordinal_10688 (bytes at +0x151)
    const uint32_t* bitMasks;                   // dword[] at D2Client+0xCEE50 deref'd (gdwBitMasks)
};
extern "C" {
// 1.13c D2Client+0x6DA40
EMSCRIPTEN_KEEPALIVE int32_t d2c_6da40(const D2c6DA40Env* e, int32_t idx, const uint32_t* esi) {
    if (idx >= 0x10) return 0;
    uint32_t id = esi ? esi[1] : 0xffffffffu;
    // Ordinal_10350 (assert/log, no result used) omitted.
    switch ((uint32_t)e->ord10175(esi)) {
    case 0: return (idx >= 0 && idx <= 4) || idx == 8 || idx == 9;
    case 1: { const uint8_t* p = e->f1260(id);
        return p && (e->bitMasks[idx & 7] & p[(idx >> 3) + 0xe8]) ? 1 : 0; }
    case 2: { const uint8_t* p = e->ord10688(id); return p && p[0x151 + idx] == 1; }
    case 4: return idx == 1;
    }
    return 0;
}
}
