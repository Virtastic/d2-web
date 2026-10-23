// D2Client+0xA6EF0: remove (key,type) node from a hash-bucket chain; env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA6EF0Env {
    void (*f8D370)(uint32_t* node);  // D2Client+0x8D370 (type 4 teardown)
    void (*fA6D60)(uint32_t* node);  // D2Client+0xA6D60 (node release)
};
// node dwords: [0]=type, [3]=key, [0x39]=next (byte 0xE4); table is a dword array of bucket heads
static inline uint32_t* nodeAt(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
extern "C" {
// 1.13c D2Client+0xA6EF0
EMSCRIPTEN_KEEPALIVE void d2c_a6ef0(const D2cA6EF0Env* e, uint32_t* table, uint32_t bucket, uint32_t key, uint32_t type) {
    uint32_t* n = nodeAt(table[bucket]);
    uint32_t* prev = 0;
    if (!n) return;
    while (n[3] != key || n[0] != type) {
        prev = n;
        n = nodeAt(n[0x39]);
        if (!n) return;
    }
    if (!prev) table[bucket] = n[0x39]; else prev[0x39] = n[0x39];
    if (n[0] == 4) e->f8D370(n);
    e->fA6D60(n);
}
}
