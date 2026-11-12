// 1.13c D2Client+0xC5730 spawnBurstRing: for `count` steps build a 23-dword request (type 3, unit, p2, the unit's size from 0xA1340, a direction pair
// from two 8-entry tables, two stack words, a callback) and hand it to 0xB30F0. Disassembly: EAX = unit, EDX = p2, stack count, a, b (ret 0xC).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cC5730Env {
    uint32_t (*a1340)(uint32_t unit, uint32_t* w, uint32_t* h);   // EBX=unit, EDI=&w, stack &h
    void (*b30f0)(uint32_t* req);                                  // ECX=&request, plain ret
};
extern "C" {
// req layout (dwords): 0 type, 1 unit, 4 p2, 5 w, 6 h, 7 dx, 8 dy, 11 a, 12 b, 21 callback (D2Client+0x73B60, 0x6FB23B60, passed as cb), 22 step index.
EMSCRIPTEN_KEEPALIVE uint32_t d2c_c5730(const D2cC5730Env* e, uint32_t unit, uint32_t p2, int32_t count, uint32_t a, uint32_t b, uint32_t cb) {
    static const int32_t dx[8] = { 0, 0, 1, -1, 1, -1, -1, 1 };    // D2Client+0xD7078
    static const int32_t dy[8] = { 1, -1, 0, 0, 1, -1, 1, -1 };    // D2Client+0xD7098
    uint32_t req[23] = {};
    req[0] = 3; req[1] = unit; req[4] = p2; req[11] = a; req[12] = b; req[21] = cb;
    if (!e->a1340(unit, &req[5], &req[6])) return 0;
    for (int32_t i = 0; i < count; ++i) {
        req[7] = (uint32_t)dx[i & 7]; req[8] = (uint32_t)dy[i & 7]; req[22] = (uint32_t)i;
        e->b30f0(req);
    }
    return 1;
}
}
