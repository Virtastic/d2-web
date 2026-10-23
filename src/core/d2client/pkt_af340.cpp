// D2Client+0xAF340: builds a 6-dword record from a packet, forwards to sub_2A750; env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cAF340Env {
    void (*f2A750)(uint32_t a, int32_t* ecx, const uint32_t* rec);  // D2Client+0x2A750
};
extern "C" {
// 1.13c D2Client+0xAF340
EMSCRIPTEN_KEEPALIVE void d2c_af340(const D2cAF340Env* e, int32_t* ecx, const uint8_t* p) {
    uint32_t r[6];
    r[0] = p[6];
    r[1] = (uint32_t)p[7] | (uint32_t)p[8] << 8 | (uint32_t)p[9] << 16 | (uint32_t)p[10] << 24;
    r[2] = p[0xb];
    r[3] = 2; r[4] = 0; r[5] = 4;
    e->f2A750(p[5], ecx, r);
}
}
