// 1.13c Storm+0x3c80: resize a {cap, count, data} buffer (retail 32-bit words); in-place SMemReAlloc (flag 0x10), else SMemAlloc + copy + SMemFree.
#include <cstdint>
extern "C" void* Storm_SMemReAlloc(void* p, uint32_t size, const char* tag, int32_t line, uint32_t flags);
extern "C" void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
extern "C" void Storm13c_3C80(uint32_t* a, uint32_t n) {
    uint32_t* old = (uint32_t*)(uintptr_t)a[2];
    a[0] = n;
    uint32_t* p = (uint32_t*)Storm_SMemReAlloc(old, n * 4u, ".I", -2, 0x10);
    a[2] = (uint32_t)(uintptr_t)p;
    if (p == nullptr) {
        p = (uint32_t*)Storm_Sub1C580(n * 4u, ".I", -2, 0);
        a[2] = (uint32_t)(uintptr_t)p;
        if (old != nullptr) {
            uint32_t m = a[1] <= n ? a[1] : n;
            for (uint32_t i = 0; i < m; i++) if (p + i != nullptr) p[i] = old[i];
            Storm_Sub1B960(old, ".I", -2);
        }
    }
}
