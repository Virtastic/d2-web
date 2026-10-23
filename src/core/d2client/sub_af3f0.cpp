// D2Client+0xAF3F0: unpack a packet-ish record into a 28-byte arg block, then dispatch; env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cAF3F0Env {
    void (*f2A1B0)(int32_t* esi, uint32_t w, int32_t zero, int32_t zero2);  // D2Client+0x2A1B0 (ESI = param_1, unverified)
    uint32_t (*f2A750)(uint32_t kind, int32_t* p1, uint32_t* args);         // D2Client+0x2A750
};
extern "C" {
// 1.13c D2Client+0xAF3F0 (fastcall: ECX=param_1, EDX=rec)
EMSCRIPTEN_KEEPALIVE void d2c_af3f0(const D2cAF3F0Env* e, int32_t* p1, const uint8_t* r) {
    uint32_t a[7];
    a[0] = r[10]; a[2] = r[0xf];
    a[1] = (uint32_t)r[0xb] | (uint32_t)r[0xc] << 8 | (uint32_t)r[0xd] << 16 | (uint32_t)r[0xe] << 24;
    a[3] = (uint32_t)(int32_t)(int16_t)(r[0x12] | r[0x13] << 8);
    a[4] = r[0x11]; a[5] = r[0x10]; a[6] = r[0x14];
    e->f2A1B0(p1, (uint16_t)(r[8] | r[9] << 8), 0, 0);
    e->f2A750(r[5], p1, a);
}
}
