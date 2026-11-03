// src/core/storm/storm_3a30.cpp -- Storm.dll destructor body, one function per file.
#include <cstdint>

extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line); // 0x1B960 (storm_alloc_1b960.cpp)

// 1.13c Storm+0x3A30: release this+8 through 0x1B960 with tag ".I", line -2 (EAX = this).
extern "C" void Storm_Sub3A30(uint32_t* self) {
    if (self[2] != 0) Storm_Sub1B960((void*)(uintptr_t)self[2], ".I", -2);
}
