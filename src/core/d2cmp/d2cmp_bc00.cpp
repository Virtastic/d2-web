// Rules from cached 1.13c body. Callee 17400 (unmapped) is an injected hook; ECX (cache) passed through.
#include <cstdint>
#include <cstring>

// 1.13c D2CMP+0x0BC00
// Walks obj+0x10C records (stride 0x60) at obj+0x110; each with node (+0x5C) set is released via 17400,
// then data (+0x54) and node (+0x5C) are cleared.
extern "C" void d2cmp_bc00(void* cache, uint8_t* obj, void (*release)(void* cache)) {
    int32_t n = *(int32_t*)(obj + 0x10C);
    for (int32_t i = 0; i < n; i++) {
        uint8_t* r = *(uint8_t**)(obj + 0x110) + i * 0x60;
        if (*(int32_t*)(r + 0x5C)) {
            release(cache);
            *(int32_t*)(r + 0x54) = 0;
            *(int32_t*)(r + 0x5C) = 0;
        }
        n = *(int32_t*)(obj + 0x10C);
    }
}
