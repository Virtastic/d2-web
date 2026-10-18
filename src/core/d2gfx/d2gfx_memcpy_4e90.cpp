// src/core/d2gfx/d2gfx_memcpy_4e90.cpp -- 1.13c D2Gfx.dll _memcpy, reimplemented from the cached
// decompile (ghidra-d2gfx/decomp/0x4E90.c). Alpha batch lane (OBJECTIVE v8.3): implementation + marker
// only, no spec, no vector.
#include <cstddef>
#include <cstring>

// 1.13c D2Gfx+0x04E90 (672 B): _memcpy(dst, src, size). The backward path (source below destination
// and the ranges overlapping) makes this the overlap-safe copy, i.e. memmove; the forward path is the
// word-at-a-time copy. The host's memmove has the same contract for every length and alignment, so it
// is the whole rule. Returns the destination base.
extern "C"
void* d2gfx_sub_4E90(void* dst, const void* src, size_t size) {
    return std::memmove(dst, src, size);
}
