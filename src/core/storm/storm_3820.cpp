// src/core/storm/storm_3820.cpp -- Storm.dll element destructor, one function per file.
#include <cstdint>

extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line); // 0x1B960 (storm_alloc_1b960.cpp)

// 1.13c Storm+0x3820: element destructor (decompile 0x3820.c, SEH wrapper stripped): release this+8 through 0x1B960 with tag ".I", line -2.
extern "C" void Storm_Sub3820(void* p) {
    uint32_t* self = (uint32_t*)p;
    if (self[2] != 0) Storm_Sub1B960((void*)(uintptr_t)self[2], ".I", -2);
}
