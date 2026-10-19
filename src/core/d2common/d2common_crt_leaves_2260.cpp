// Prior art used: 's D2MOO map; D2MOO delegates these CRT
// operations to the host library (e.g. D2QuestRecord.cpp's memcpy). No matching
// CRT bodies are supplied there. The cached 1.13c bodies determine the rules,
// including memcpy's backward-copy overlap path. Alignment/unrolling changes
// access granularity only; byte loops preserve the defined buffer results.
#include <cstdint>

// 1.13c D2Common+0x02260: search includes the terminating NUL and narrows the key.
extern "C" char* d2common_strchr_2260(char* text, int32_t value)
{
    const uint8_t key = static_cast<uint8_t>(value);
    for (;;) {
        const uint8_t byte = static_cast<uint8_t>(*text);
        if (byte == key) return text;
        if (byte == 0) return nullptr;
        ++text;
    }
}

// 1.13c D2Common+0x048D0: retail's memcpy also handles overlapping ranges.
// Pointers keep native width; only the retail size argument is a 32-bit word.
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

// 1.13c D2Common+0x04DD0: unsigned EDX:EAX quotient of two retail word pairs.
extern "C" uint64_t d2common_aulldiv_4dd0(uint32_t low, uint32_t high,
                                        uint32_t divisorLow, uint32_t divisorHigh)
{
    const uint64_t numerator = (uint64_t{high} << 32) | low;
    const uint64_t denominator = (uint64_t{divisorHigh} << 32) | divisorLow;
    // The x86 DIV faults for zero; avoid C++ undefined behavior on that path.
    if (denominator == 0) __builtin_trap();
    return numerator / denominator;
}

// 1.13c D2Common+0x06A60: fill with the low byte, returning the original pointer.
extern "C" void* d2common_memset_6a60(void* destination, int32_t value, uint32_t size)
{
    auto* out = static_cast<uint8_t*>(destination);
    for (uint32_t i = 0; i < size; ++i) out[i] = static_cast<uint8_t>(value);
    return destination;
}

// 1.13c D2Common+0x06B50: pointer traversal is native, result wraps as retail size_t.
extern "C" uint32_t d2common_strlen_6b50(const char* text)
{
    uint32_t length = 0;
    while (*text++ != '\0') ++length;
    return length;
}
