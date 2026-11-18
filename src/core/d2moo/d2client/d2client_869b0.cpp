// 1.13c D2Client+0x869B0: ESI = packet (byte +2 = total length). Calls the Ordinal 11105
// thunk (0xCA90, stdcall 4 words: data = packet+0xD, length = byte2 - 0xD, 0, &local24) which fills a 24-byte local,
// then 0x85B30 (EAX = packet, stack = &local, ret 4). Both callees are injected.
#include <cstdint>

struct D2c869b0Env {
    void (*ord11105)(const uint8_t* data, int32_t len, uint32_t zero, uint8_t* local);   // thunk 0xCA90
    void (*c85b30)(const uint8_t* esi, uint8_t* local);                                   // 0x85B30
};

extern "C" void d2c_port_869b0(const uint8_t* esi, const D2c869b0Env& e) {
    uint8_t local[24];
    e.ord11105(esi + 0xD, (int32_t)esi[2] - 0xD, 0, local);
    e.c85b30(esi, local);
}
