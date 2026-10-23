// 1.13c Fog+0x07E41: MSVC CRT _calloc_impl(num, size). Overflow guard (size <= 0xFFFFFFE0/num),
// zeroed block, size 0 -> 1 byte; retail's small-block heap/HeapAlloc/new-handler retry collapse to calloc.
#include <cstdint>
#include <cstdlib>

extern "C" void* fog_calloc_7e41(uint32_t num, uint32_t size) {  // 1.13c Fog+0x07E41
    if (num != 0 && size > 0xFFFFFFE0u / num) return nullptr;
    uint32_t n = num * size;
    return calloc(n ? n : 1, 1);
}
