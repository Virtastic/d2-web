// src/core/d2cmp/d2cmp_batch_1920.cpp -- alpha-lane batch: D2CMP rows from the cached Ghidra decompiles.
// No spec, no vector. Skipped (not faithful without disassembly/state): 0x4E66 (register-passed),
// 0x56A5 (CRT small-block heap on unported globals), 0x6E88 (CRT MessageBox loader), 0x7875 (stack-addr tricks).
#include <cstdint>
#include <Windows.h>

struct D2CmpEnv1920 {
    void (*f1F30)();            // 0x1F30
    void (*f2DD8)(int32_t n);   // 0x2DD8
};

// 1.13c D2CMP+0x1920
void d2cmp_1920(const D2CmpEnv1920* e, uint32_t* self) {
    e->f1F30();
    uint32_t first = self[1];
    if (!first) return;
    uint32_t prev = self[2];
    uint32_t* slot = ((int32_t)prev < 0) ? (uint32_t*)(uintptr_t)~prev
                                         : (uint32_t*)((uintptr_t)(self + 1) + (prev - ((uint32_t*)(uintptr_t)first)[1]));
    *slot = first;
    ((uint32_t*)(uintptr_t)self[1])[1] = self[2];
    self[1] = 0;
    self[2] = 0;
}

// 1.13c D2CMP+0x3d9d
void d2cmp_3d9d(const D2CmpEnv1920* e) { e->f2DD8(4); }
// 1.13c D2CMP+0x41B9
void d2cmp_41b9(const D2CmpEnv1920* e) { e->f2DD8(4); }
// 1.13c D2CMP+0x6830
void d2cmp_6830(const D2CmpEnv1920* e) { e->f2DD8(0xc); }

// 1.13c D2CMP+0x4C6D  ___crtInitCritSecNoSpinCount@8
uint32_t d2cmp_4c6d(LPCRITICAL_SECTION cs) { InitializeCriticalSection(cs); return 1; }

// 1.13c D2CMP+0x6D90  strcpy
char* d2cmp_6d90(char* dst, const char* src) {
    char* d = dst;
    while ((*d++ = *src++) != 0) {}
    return dst;
}

// 1.13c D2CMP+0x6da0  strcat
char* d2cmp_6da0(char* dst, const char* src) {
    char* d = dst;
    while (*d) d++;
    while ((*d++ = *src++) != 0) {}
    return dst;
}

// 1.13c D2CMP+0x6f90  strncpy (zero-pads to count)
char* d2cmp_6f90(char* dst, const char* src, uint32_t n) {
    uint32_t i = 0;
    for (; i < n && src[i]; i++) dst[i] = src[i];
    for (; i < n; i++) dst[i] = 0;
    return dst;
}
