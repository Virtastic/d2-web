#include <emscripten.h>
#include <cstdint>
#include <cstring>

// 1.13c D2Client+0x1C740 (ECX = packet): when the u16 at +1 is 1, the u16s at +3/+5/+7 become the three
// dwords at 0x1223E4/E8/EC. The dwords are passed in.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1c740(const uint8_t* p, uint32_t* g) {
    uint16_t k, v[3];
    std::memcpy(&k, p + 1, 2);
    if (k != 1) return;
    std::memcpy(v, p + 3, 6);
    g[0] = v[0]; g[1] = v[1]; g[2] = v[2];
}
