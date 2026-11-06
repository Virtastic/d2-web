// D2Client+0x3A3A0: clears 8 state dwords, scans the 10-byte-stride table at 0x6FBCA2E0 (0x474 bytes). Each entry
// {int32 id, uint16 w, ...} with w in 0x100..0x104 and id < 0x39 selects variant c = w-0x100: it asks a predicate
// (0x42060, 0x42030, or 0x420E0 then 0x42090) and on success copies the id's 12-byte cost record at 0x6FBA4F10 into
// the globals, else (also when the cost +4 test fails for c3/c4) sets w := 0xFFFF. The scan continues; nothing tail-calls.
// Predicates are seams (they read the game-mode globals and GetSystemMetrics).
#include <cstdint>
#include <emscripten.h>
struct D2c3A3A0Env {
    int32_t* g[8];                       // dwords at 6FBCA75C,6FBCA760,6FBC9E60,6FBCA764,6FBCA76C,6FBCA754,6FBCA768,6FBCA758 (in clear order)
    uint8_t* table;                      // bytes at 6FBCA2E0 (0x474 bytes), w is rewritten in place
    const int32_t* cost;                 // 12-byte records at 6FBA4F10, index = id
    int (*p42060)();
    int (*p42030)();
    int (*p420e0)();
    int (*p42090)();
};
extern "C" {
// 1.13c D2Client+0x3A3A0
EMSCRIPTEN_KEEPALIVE void d2c_3a3a0(const D2c3A3A0Env* e) {
    for (int i = 0; i < 8; i++) *e->g[i] = 0;
    for (uint8_t* p = e->table; p < e->table + 0x474; p += 10) {
        int32_t id = *(const int32_t*)p;
        uint16_t w = *(const uint16_t*)(p + 4);
        if (w <= 0xdf || id >= 0x39) continue;
        uint32_t c = (uint32_t)w - 0x100;
        if (c > 4) continue;
        const int32_t* k = e->cost + id * 3;
        bool ok;
        switch (c) {
        case 0: ok = e->p42060() != 0; if (ok) { *e->g[2] = k[0]; *e->g[3] = k[1]; } break;
        case 1: ok = e->p42030() != 0; if (ok) { *e->g[4] = k[0]; *e->g[5] = k[1]; } break;
        case 2: ok = e->p42030() != 0; if (ok) { *e->g[6] = k[0]; *e->g[7] = k[1]; } break;
        default:
            ok = e->p420e0() != 0 || e->p42090() != 0;
            if (ok && k[1] != 0) ok = false;
            if (ok) *e->g[c == 3 ? 0 : 1] = k[0];
            break;
        }
        if (!ok) *(uint16_t*)(p + 4) = 0xffff;
    }
}
}
