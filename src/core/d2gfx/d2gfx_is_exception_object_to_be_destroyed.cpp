// 1.13c D2Gfx+0x0BED3 IsExceptionObjectToBeDestroyed (33 bytes), from ghidra-d2gfx/decomp/0xBED3.c.
// Walks the per-thread FrameInfo chain (_curexception at ptd+0x88; FrameInfo {+0 obj, +4 next}); returns 0 if
// obj is on it (still in flight), else 1. ptd = __getptd() result, passed in (callee 0x28FE is a seam).
#include <cstdint>
// 1.13c D2Gfx+0xBED3
extern "C" int d2gfx_is_exception_object_to_be_destroyed(const uint8_t* mem, uint32_t ptd, uint32_t obj) {
    uint32_t fi = *(const uint32_t*)(mem + ptd + 0x88);
    while (fi) {
        if (*(const uint32_t*)(mem + fi) == obj) return 0;
        fi = *(const uint32_t*)(mem + fi + 4);
    }
    return 1;
}
