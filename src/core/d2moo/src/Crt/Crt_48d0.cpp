// 1.13c D2Common+0x48D0 memcpy: retail's memcpy also handles overlapping ranges (backward copy). D2MOO has no twin (host CRT).
// Pointers keep native width; only the retail size argument is a 32-bit word. Alignment/unrolling changes access granularity
// only; byte loops preserve the defined buffer results.
#include <cstdint>

// 1.13c D2Common+0x48D0
extern "C" void* d2common_memcpy_48d0(void* destination, const void* source, uint32_t size)
{
    auto* out = static_cast<uint8_t*>(destination);
    const auto* in = static_cast<const uint8_t*>(source);
    const uintptr_t dst = reinterpret_cast<uintptr_t>(out);
    const uintptr_t src = reinterpret_cast<uintptr_t>(in);
    if (dst > src && dst - src < size) {
        for (uint32_t i = size; i != 0; --i) out[i - 1] = in[i - 1];
    } else {
        for (uint32_t i = 0; i < size; ++i) out[i] = in[i];
    }
    return destination;
}
