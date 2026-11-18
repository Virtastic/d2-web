// src/core/storm/storm_4040.cpp -- 1.13c Storm+0x4040 (StormRefPairRelease_4040), retail layout.
// Child P at +0xC (refcount +0x140) releases through 0x3EF0 and frees "delete"; child R at +0x30 (refcount +0x4C) through 0x3FC0; 0x4B80(O) last.
#include <cstdint>
void Storm_Sub3EF0(uint32_t p);
void Storm_Sub3FC0(uint32_t p);
extern "C" void Storm_Sub4B80(uint32_t p);
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line); // 0x1B960 (storm_alloc_1b960.cpp)
static inline void Storm_Free(uint32_t p, const char* file, int32_t line) { (void)Storm_Sub1B960((void*)(uintptr_t)p, file, line); }

static inline uint32_t& w(uint32_t base, uint32_t off) { return *(uint32_t*)(uintptr_t)(base + off); }

extern "C" void Storm_Sub4040(uint32_t O) {
    uint32_t P = w(O, 0x0c);
    if (P && --w(P, 0x140) == 0) { Storm_Sub3EF0(P); Storm_Free(P, "delete", -1); }
    uint32_t R = w(O, 0x30);
    if (R && --w(R, 0x4c) == 0) { Storm_Sub3FC0(R); Storm_Free(R, "delete", -1); }
    Storm_Sub4B80(O);
}
