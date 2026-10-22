// src/core/d2launch/d2launch_crt_b52.cpp -- MSVC CRT leaves of 1.13c D2Launch.dll (alpha lane).
#include <cstdint>
#include <cstddef>
#include <cstring>

// 1.13c D2Launch+0x04350 -- __aulldiv (unsigned 64-bit divide, stdcall four dwords)
extern "C" uint64_t d2launch_aulldiv(uint32_t lo, uint32_t hi, uint32_t dlo, uint32_t dhi) {
    return (((uint64_t)hi << 32) | lo) / (((uint64_t)dhi << 32) | dlo);
}

// 1.13c D2Launch+0x04010 -- _memcpy (copies backward on forward overlap, i.e. memmove semantics)
extern "C" void* d2launch_memcpy(void* dst, const void* src, size_t n) {
    return memmove(dst, src, n);
}
