// 1.13c D2Common+0x6A60 memset: fill with the low byte, returning the original pointer. D2MOO has no twin (host CRT).
#include <cstdint>

// 1.13c D2Common+0x6A60
extern "C" void* d2common_memset_6a60(void* destination, int32_t value, uint32_t size)
{
    auto* out = static_cast<uint8_t*>(destination);
    for (uint32_t i = 0; i < size; ++i) out[i] = static_cast<uint8_t>(value);
    return destination;
}
