// 1.13c BNClient+0x0A5B0 -- SRP object allocator (EDI = out slot, 32-bit in retail): Storm #401 (thunk 0x8018) with
// (0x4C, "..\Source\SRP\SRP.cpp", 0x157, 0); on success dwords 0 and 0x12 are zeroed; the result (or 0) is stored through *out.
#include <cstdint>

typedef void* (*BnSmemAllocA5B0Fn)(uint32_t size, const char* file, uint32_t line, uint32_t flags);

extern "C" void d2_bnclient_a5b0(uint32_t* out, BnSmemAllocA5B0Fn smem_alloc) {
    uint32_t* p = (uint32_t*)smem_alloc(0x4C, "..\\Source\\SRP\\SRP.cpp", 0x157, 0);
    if (p) { p[0] = 0; p[0x12] = 0; }
    *out = (uint32_t)(uintptr_t)p;
}
