// 1.13c Storm+0x11C70 / 0x11C30: the SCOMP read and write callbacks over a six-word context {dst, written, cap, src, pos, srcSize}.
// Context words are retail 32-bit addresses and counts.
#pragma once
#include <cstdint>
#include <cstring>

namespace scomp {
inline uint32_t readCb(uint8_t* buf, uint32_t* size, void* user) {   // 0x11C70
    uint32_t* c = (uint32_t*)user;
    uint32_t n = c[5] - c[4];
    if (*size < n) n = *size;
    std::memcpy(buf, (const void*)(uintptr_t)(c[3] + c[4]), n);
    c[4] += n;
    return n;
}
inline void writeCb(uint8_t* buf, uint32_t* size, void* user) {      // 0x11C30
    uint32_t* c = (uint32_t*)user;
    uint32_t n = c[2] - c[1];
    if (*size < n) n = *size;
    std::memcpy((void*)(uintptr_t)(c[0] + c[1]), buf, n);
    c[1] += n;
}
inline uint32_t cb32(uint32_t (*f)(uint8_t*, uint32_t*, void*)) { return (uint32_t)(uintptr_t)f; }
inline uint32_t cb32(void (*f)(uint8_t*, uint32_t*, void*)) { return (uint32_t)(uintptr_t)f; }
}
