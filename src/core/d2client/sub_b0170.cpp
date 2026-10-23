// D2Client+0xB0170: register-arg helper (ESI=unit); env-seam style. Ordinal numbers unverified (mapped by callee order C424,C43C,C3F4,C3AC,C400).
#include <cstdint>
#include <emscripten.h>
struct D2cB0170Env {
    uint8_t* recBase;      // *(sgptDataTables+0xb64): 0x1a4-byte records
    int32_t recCount;      // *(sgptDataTables+0xb6c)
    uint32_t bitMask16;    // gdwBitMasks[0x10]
    const uint32_t* tab8d85c;  // DAT 6fb8d85c, 8-byte stride, dword at +0
    void (*ord10098)();    // via C424 (unverified)
    void (*ord11090)();    // via C43C (unverified)
    void (*ord11003)(int32_t zero);  // via C3F4 (unverified)
    void (*ord11056)(uint32_t path, uint32_t v);  // via C3AC (unverified)
    void (*ord11164)(uint32_t path, uint32_t v);  // via C400 (unverified)
};
extern "C" {
// 1.13c D2Client+0xB0170
EMSCRIPTEN_KEEPALIVE void d2c_b0170(const D2cB0170Env* e, uint32_t* esi) {
    if (!esi) return;
    int32_t idx = (int32_t)esi[1];
    if (idx < 0 || idx >= e->recCount) return;
    const uint8_t* rec = e->recBase + (uint32_t)idx * 0x1a4;
    if (!rec) return;
    e->ord10098(); e->ord11090(); e->ord11003(0);
    uint32_t path = esi[0x0b];
    e->ord11056(path, rec[0x184] ? 0x40 : 0);
    e->ord11164(path, e->tab8d85c[(uint32_t)rec[0x183] * 2]);
    uint32_t f = (e->bitMask16 & rec[4]) ? (esi[0x31] | 4) : (esi[0x31] & 0xfffffffb);
    esi[0x31] = (f & 0xfffffff5) | 0x20;
}
}
