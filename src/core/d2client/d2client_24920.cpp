// Object slot owner: ECX = object, AX = count, stack a0 a1 a2 (ret 0xC).
// The node link at obj+0x64 is a 32-bit retail address.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2c24920Env {
    uint32_t (*alloc)(uint32_t half, uint32_t ff, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t ebx); // +A8C20
    void (*release)(uint32_t node);                                                                     // +A8630, ESI
    void (*init)(uint32_t half, uint32_t node);                                                         // +A8BE0, EAX, ECX
};
// 1.13c D2Client+0x24920
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_24920(uint8_t* obj, uint32_t count, uint32_t a0, uint32_t a1, uint32_t a2, const D2c24920Env* e) {
    const uint16_t ax = static_cast<uint16_t>(count);
    uint32_t node; std::memcpy(&node, obj + 0x64, 4);
    if (ax == 0) {
        if (!node) return;
        e->release(node);
        node = 0; std::memcpy(obj + 0x64, &node, 4);
        return;
    }
    if (!node) {
        node = e->alloc(ax >> 1, 0xff, a0, a1, a2, 2);
        std::memcpy(obj + 0x64, &node, 4);
        if (!node) return;
    }
    e->init(ax >> 1, node);
}
