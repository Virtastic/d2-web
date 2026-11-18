// 1.13c D2Client+0x2A040: handle = obj[+0x2c]; call 0xC3EE(handle, ecx, eax); call 0xC376(handle, obj, (obj[+0xc4] >> 21) & 1).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" EMSCRIPTEN_KEEPALIVE void D2Client_ObjSync2A040(const uint8_t* obj, uint32_t eax, uint32_t ecx,
        void (*c3ee)(uint32_t, uint32_t, uint32_t), void (*c376)(uint32_t, const uint8_t*, uint32_t)) {
    uint32_t h, f; memcpy(&h, obj + 0x2c, 4); memcpy(&f, obj + 0xc4, 4);
    c3ee(h, ecx, eax);
    c376(h, obj, (f >> 21) & 1);
}
