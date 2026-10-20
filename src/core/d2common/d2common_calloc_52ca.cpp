#include <cstdint>
#include <cstdlib>

// MSVCRT calloc: overflow guard (0xffffffe0/count), zero-filled block, size 0 -> 1 byte.
// Retail's small-block heap / HeapAlloc / _callnewh retry loop is the host allocator here.
// 1.13c D2Common+0x052CA
void* d2common_calloc_52ca(uint32_t count, uint32_t size)
{
    if (count != 0 && size > 0xffffffe0u / count)
        return nullptr;
    uint32_t n = count * size;
    return std::calloc(n ? n : 1, 1);
}
