// src/core/d2win/d2win_sub_2DDA.cpp -- 1.13c D2Win+0x02DDA, the statically linked MSVCRT calloc(count, size),
// from the cached decompile. Contract: 0 on overflow (count != 0 && size > 0xFFFFFFE0 / count); a zero
// request allocates 1 byte; result is zero-filled. Small-block heap / HeapAlloc / new-handler retry
// collapse to calloc (host allocator, no new-handler installed).
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE void* d2win_sub_2DDA(uint32_t count, uint32_t size) {
    if (count && size > 0xFFFFFFE0u / count) return nullptr;
    uint32_t n = count * size;
    return std::calloc(n ? n : 1, 1);
}
