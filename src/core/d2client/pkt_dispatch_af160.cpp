// D2Client+0xAF160: __fastcall(ECX=param_1, EDX=packet); env-seam style. Builds a 5-dword record and calls D2Client+0x2A750(0x15, p1, rec).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2cAF160Env {
    void (*f2A750)(int32_t kind, void* p1, const int32_t* rec);  // D2Client+0x2A750
};
extern "C" {
// 1.13c D2Client+0xAF160
EMSCRIPTEN_KEEPALIVE void d2c_af160(const D2cAF160Env* e, void* p1, const uint8_t* pkt) {
    if (!p1) return;
    uint16_t w0, w1; memcpy(&w0, pkt + 0xb, 2); memcpy(&w1, pkt + 0xd, 2);
    int32_t r[7] = {0};
    memcpy(&r[0], pkt + 6, 4);
    r[1] = -1;
    r[2] = w0;
    r[3] = w1;
    r[4] = pkt[10];
    e->f2A750(0x15, p1, r);
}
}
