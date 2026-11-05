// Record append: when the current node already holds 500 records (signed count at +0x1F40) a fresh node is chained first;
// then the record at count*16 gets {stamp, a, b, c}. The count is bumped before the record is written, so the slot past the
// last one aliases the count itself, exactly as the original.
#include <cstdint>
#include <cstring>
struct D2cABA10Env {
    uint8_t* (*node)(void* ctx);
    uint32_t (*stamp)(void* ctx);
    void (*grow)(void* ctx);
    void* ctx;
};
extern "C" void d2c_aba10(uint32_t a, uint32_t b, uint32_t c, const D2cABA10Env* e) {
    uint8_t* n = e->node(e->ctx);
    int32_t k; std::memcpy(&k, n + 0x1F40, 4);
    if (k >= 500) { e->grow(e->ctx); n = e->node(e->ctx); std::memcpy(&k, n + 0x1F40, 4); }
    uint8_t* rec = n + (uint32_t)k * 16u;
    const int32_t k1 = k + 1; std::memcpy(n + 0x1F40, &k1, 4);
    const uint32_t v[4] = { e->stamp(e->ctx), a, b, c };
    for (int i = 0; i < 4; ++i) std::memcpy(rec + 4 * i, &v[i], 4);
}
