// 1.13c BNClient+0xA5B0
// SRP object allocator: SMemAlloc(0x4C, "..\Source\SRP\SRP.cpp", 0x157, 0) via thunk 0x8018. On success
// dword[0] and dword[0x12] are zeroed; the result (or NULL) is stored through *out (retail: EDI).
#include <cstdint>

typedef void* (*BnSmemAllocA5B0Fn)(uint32_t size, const char* file, uint32_t line, uint32_t flags);

extern "C" void d2_bnclient_a5b0(void** out, BnSmemAllocA5B0Fn smem_alloc) {
    uint32_t* p = (uint32_t*)smem_alloc(0x4C, "..\\Source\\SRP\\SRP.cpp", 0x157, 0);
    if (p) { p[0] = 0; p[0x12] = 0; }
    *out = p;
}
