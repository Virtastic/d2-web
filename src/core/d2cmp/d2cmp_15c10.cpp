// Rules from cached 1.13c body. Null object path (assert message + _exit(-1)) is an injected hook.
#include <cstdint>

// 1.13c D2CMP+0x15C10
// Relocates entries (stride 0x14, count obj+0x50, table obj+0x54): entry+0x10 += table base.
extern "C" void d2cmp_15c10(uint8_t* obj, void (*fatal)()) {
    if (!obj) { fatal(); return; }
    uint8_t* base = *(uint8_t**)(obj + 0x54);
    int32_t n = *(int32_t*)(obj + 0x50);
    for (int32_t i = 0; i < n; i++)
        *(int32_t*)(*(uint8_t**)(obj + 0x54) + i * 0x14 + 0x10) += (int32_t)(intptr_t)base;
}
