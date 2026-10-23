// src/core/d2win/d2win_sub_4366.cpp -- D2Win VC7.1 CRT _realloc.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

// 1.13c D2Win+0x4366 (_realloc): null ptr -> malloc(size); size 0 -> free, returns null; else resize.
// Retail's small-block-heap / HeapReAlloc / _callnewh retry paths are the Win32 allocator's internals;
// libc realloc stands in (ponytail: no new-handler retry loop, wasm has no _set_new_handler).
// Sizes above 0xFFFFFFE0 fail (retail calls _callnewh, returns null).
extern "C" EMSCRIPTEN_KEEPALIVE void *d2win_sub_4366(void *p, uint32_t size) {
    if (!p) return malloc(size);
    if (!size) { free(p); return nullptr; }
    if (size > 0xFFFFFFE0u) return nullptr;
    return realloc(p, size);
}
