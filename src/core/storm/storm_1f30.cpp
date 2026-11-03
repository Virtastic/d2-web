// 1.13c Storm+0x1f30: TSFixedArray<16-byte element>::Set over the retail layout {alloc, count, data} (32-bit words).
#include <cstdint>
#include <cstring>
extern "C" void* Storm_SMemReAlloc(void* p, uint32_t size, const char* tag, int32_t line, uint32_t flags);
extern "C" void Storm13c_1F30(uint32_t* a, uint32_t count, const uint8_t* src) {
    a[0] = count;
    if (a[2] != 0 || count != 0)
        a[2] = (uint32_t)(uintptr_t)Storm_SMemReAlloc((void*)(uintptr_t)a[2], count * 16u, "AURECTF", -2, 0);
    for (uint32_t i = 0; i < count; i++) {
        uint8_t* d = (uint8_t*)(uintptr_t)a[2] + (uintptr_t)i * 16u;
        if (d != nullptr) memcpy(d, src + (uintptr_t)i * 16u, 16u);
    }
    a[1] = count;
}
