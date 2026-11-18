// D2Client+0xAD680: __fastcall(ECX=pkt); copies 40 bytes to a stack buffer, calls 0x7DE50 with EAX=copy.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2cAD680Env {
    void (*f7DE50)(const uint8_t* pkt);  // D2Client+0x7DE50 (EAX = packet)
};
extern "C" {
// 1.13c D2Client+0xAD680
EMSCRIPTEN_KEEPALIVE void d2c_ad680(const D2cAD680Env* e, const uint8_t* pkt) {
    uint8_t copy[40];
    memcpy(copy, pkt, 40);
    e->f7DE50(copy);
}
}
