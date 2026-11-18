// Message filter, fastcall(ECX = message). Words: +4 id, +0x10 param, +0x14 points to a pointer to the state record (byte +0x1B5).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2c25b70Env {
    const uint8_t* (*mem)(uint32_t);                                        // retail address to pointer
    void (*send)(const uint32_t* msg, uint32_t eax, uint32_t three);        // +6B300, stack msg, 3, 0 x5
};
static uint32_t w32(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
// 1.13c D2Client+0x25B70
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_25b70(const uint32_t* m, const D2c25b70Env* e) {
    const uint8_t* pp = e->mem(m[5]);
    const uint8_t* obj = e->mem(w32(pp));
    if (!obj[0x1b5]) return;
    const uint32_t id = m[1], p = m[4];
    if (id == 0x95) { if (p == 1) e->send(m, 0xb6, 3); return; }
    if (id == 0x98) { e->send(m, 0xb6, 3); return; }
    if (p != 0) return;
    if (id == 0x18d || (id >= 0x162 && id <= 0x164) || (id >= 0x195 && id <= 0x197) || id == 0x1c7) e->send(m, 0x47, 3);
}
