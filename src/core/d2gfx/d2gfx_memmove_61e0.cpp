// src/core/d2gfx/d2gfx_memmove_61e0.cpp -- 1.13c D2Gfx.dll _memmove, reimplemented from the cached
// decompile (ghidra-d2gfx/decomp/0x61E0.c). Alpha batch lane (OBJECTIVE v8.3): implementation + marker
// only, no spec, no vector.
//
// Prior art: src/core/d2gfx/d2gfx_memcpy_4e90.cpp (same VC7.1 shape in this DLL) and
// src/core/realm/mcp_crt_mem.cpp 0x5260 (size 672, byte-identical routine).
#include <cstddef>
#include <cstring>

// 1.13c D2Gfx+0x061E0 (672 B): _memmove(dst, src, size). When src < dst and dst < src + size the copy
// runs backwards from the tail in dwords with a byte tail; otherwise forwards. The alignment-driven
// split into byte/word runs is unobservable: every path writes exactly the bytes memmove writes for the
// same arguments. Returns the destination base.
extern "C"
void* d2gfx_memmove_61E0(void* dst, const void* src, size_t size) {
    return std::memmove(dst, src, size);
}
