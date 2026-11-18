// 1.13c D2Client+0x7BF60 (71 B): EDI = text block. y (+8) = 0, x (+0xC) = the u16 at +0x14; the nodes {text, next} chained from +0 are measured
// through 0xD35A (ECX = text); w (+0x10, u16) = max(0, widest) + 0x14, h (+0x12, u16) = (u16)(count (u16 at +4) * 15) + 6.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2c7BF60Env { int32_t (*textWidth)(uint32_t text); };   // 0xD35A (ECX)

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_port_7bf60(uint8_t* b, const D2c7BF60Env* env) {
    uint16_t rate, count;
    uint32_t node;
    std::memcpy(&rate, b + 0x14, 2);
    std::memcpy(&node, b, 4);
    std::memcpy(&count, b + 4, 2);
    const uint32_t zero = 0, x = rate;
    std::memcpy(b + 8, &zero, 4);
    std::memcpy(b + 0xc, &x, 4);
    int32_t mx = 0;
    while (node) {
        uint32_t text, next;
        std::memcpy(&text, (const void*)(uintptr_t)node, 4);
        std::memcpy(&next, (const void*)(uintptr_t)(node + 4), 4);
        const int32_t w = env->textWidth(text);
        if (!(mx > w)) mx = w;
        node = next;
    }
    const uint16_t ww = (uint16_t)((uint32_t)mx + 0x14u);
    const uint16_t hh = (uint16_t)((uint16_t)(count * 15) + 6);
    std::memcpy(b + 0x10, &ww, 2);
    std::memcpy(b + 0x12, &hh, 2);
}
