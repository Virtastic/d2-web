#include <cstdint>

// 1.13c D2Gfx+0x05890
// CRT byte fill. Native word stores are avoided so arbitrary byte buffers need no
// alignment or effective-type assumptions; retail's aligned dword loop writes
// the same bytes and returns the original destination.
extern "C" void* d2gfx_memset(void* destination, int32_t value, uint32_t size) {
    auto* bytes = static_cast<uint8_t*>(destination);
    const uint8_t fill = static_cast<uint8_t>(value);
    for (uint32_t i = 0; i < size; ++i) bytes[i] = fill;
    return destination;
}
