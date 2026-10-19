// Prior art used: RE map; D2MOO D2StatList.cpp uses CRT
// memmove at its stat insertion/removal sites, but has no implementation of this
// CRT body. The cached 1.13c body supplies alignment, direction and word order.
#include <cstdint>

namespace {
// Byte loads avoid alignment/aliasing UB while retaining the x86 instruction's
// read-all-four-before-writing behaviour, even for one-byte overlaps. Volatile
// prevents the compiler replacing this implementation with a host CRT call.
uint32_t load_word(const volatile uint8_t* p) {
    return uint32_t(p[0]) | (uint32_t(p[1]) << 8) |
           (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}
void store_word(volatile uint8_t* p, uint32_t word) {
    p[0] = uint8_t(word); p[1] = uint8_t(word >> 8);
    p[2] = uint8_t(word >> 16); p[3] = uint8_t(word >> 24);
}
}

// 1.13c D2Common+0x02320
// cdecl ABI: return original destination; source/destination are native owner
// pointers, while the retail byte count remains explicitly 32-bit. No callbacks
// or host memmove import. Retail's <=7-word fallthrough and rep-movsd paths have
// the same ordered word transfers and are expressed as one loop here.
extern "C" void* D2Common_02320_Memmove(void* destination, const void* source,
                                        uint32_t count) {
    auto* dst = static_cast<volatile uint8_t*>(destination);
    auto* src = static_cast<const volatile uint8_t*>(source);
    const uintptr_t d = reinterpret_cast<uintptr_t>(destination);
    const uintptr_t s = reinterpret_cast<uintptr_t>(source);
    // Unsigned distance avoids overflow in source+count, and does not truncate
    // wasm64 pointers to the retail 32-bit address space.
    if (s < d && d - s < count) {
        uint32_t remaining = count;
        if (remaining >= 4) {
            uint32_t tail = uint32_t((d + remaining) & 3);
            while (tail--) { --remaining; dst[remaining] = src[remaining]; }
            while (remaining >= 4) {
                remaining -= 4;
                const uint32_t word = load_word(src + remaining);
                store_word(dst + remaining, word);
            }
        }
        while (remaining) { --remaining; dst[remaining] = src[remaining]; }
    } else {
        uint32_t offset = 0;
        uint32_t remaining = count;
        if (remaining >= 4) {
            uint32_t head = uint32_t((0 - d) & 3);
            while (head--) { dst[offset] = src[offset]; ++offset; --remaining; }
            while (remaining >= 4) {
                const uint32_t word = load_word(src + offset);
                store_word(dst + offset, word);
                offset += 4; remaining -= 4;
            }
        }
        while (remaining--) { dst[offset] = src[offset]; ++offset; }
    }
    return destination;
}
