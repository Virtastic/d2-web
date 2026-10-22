// src/core/d2cmp/d2cmp_batch_crt_b49.cpp -- 1.13c D2CMP.dll MSVC 2003 CRT rows, from the cached
// Ghidra decompiles (build-wasm/ghidra-d2cmp/decomp/0x<RVA>.c). Alpha batch lane: marker only.
// Not ported (no faithful wasm64 form): 0x4ABC (message-table box over 32-bit data), 0x4DD2 __local_unwind2
// (x86 SEH chain), 0x673D/0x67FE (tlocinfo refcounts over the 32-bit ptd layout). The small-block heap
// paths of 0x3D2B/0x40F3 are the host allocator's, as in d2cmp_batch_crt_b48.cpp.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstddef>

extern "C" void d2cmp_2dd8(uint32_t index);   // 0x2DD8 lock release
extern "C" __attribute__((weak)) void d2cmp_7875(int code);   // 0x7875 (doexit), seam

// DAT_6fe34928: the _set_new_handler callback; DAT_6fe3492c: the _set_new_mode flag.
static int (*g_newh)(size_t);
static int g_newMode;

// 1.13c D2CMP+0x2E84 (9 bytes): release CRT lock 10.
// 1.13c D2CMP+0x2E84
extern "C" void d2cmp_2e84(void) { d2cmp_2dd8(10); }

// 1.13c D2CMP+0x6315 (27 bytes): __callnewh -- 1 if the new-handler exists and returned non-zero.
// 1.13c D2CMP+0x6315
extern "C" int d2cmp_callnewh(size_t size) { return g_newh && g_newh(size) ? 1 : 0; }

// 1.13c D2CMP+0x3D2B (111 bytes): __heap_alloc -- 0 bytes becomes 1; 16-byte rounding is HeapAlloc's.
// 1.13c D2CMP+0x3D2B
extern "C" void* d2cmp_3d2b(size_t size) { return malloc(size ? size : 1); }

// 1.13c D2CMP+0x40F3 (203 bytes): _calloc_impl -- overflow guard (count*size <= 0xffffffe0), zeroed
// block, new-handler retry when the new-mode flag is set.
// 1.13c D2CMP+0x40F3
extern "C" void* d2cmp_40f3(uint32_t count, uint32_t size) {
    if (count != 0 && size > 0xffffffe0u / count) return nullptr;
    uint32_t n = count * size;
    uint32_t want = n ? n : 1;
    for (;;) {
        void* p = want < 0xffffffe1u ? calloc(1, (want + 0xf) & ~0xfu) : nullptr;
        if (p) return p;
        if (!g_newMode || !d2cmp_callnewh(want)) return nullptr;
    }
}

// 1.13c D2CMP+0x4C34 (57 bytes): __FF_MSGBANNER -- app-type globals select console; the GUI path writes nothing.
// With neither console flag set there is nothing to emit.
// 1.13c D2CMP+0x4C34
extern "C" void d2cmp_4c34(void) {}

// 1.13c D2CMP+0x4C7D (103 bytes): __crtInitCritSecAndSpinCount -- critical sections are no-ops here.
// 1.13c D2CMP+0x4C7D
extern "C" void d2cmp_4c7d(void* cs, uint32_t spin) { (void)cs; (void)spin; }

// 1.13c D2CMP+0x6232 (41 bytes): report_failure -- doexit(1) then ExitProcess(3).
// 1.13c D2CMP+0x6232
extern "C" [[noreturn]] void d2cmp_6232(void) {
    if (d2cmp_7875) d2cmp_7875(1);
    exit(3);
}

// 1.13c D2CMP+0x70C0 (672 bytes): _memmove -- overlap-safe copy, returns dst (the word/byte
// unrolling is an implementation detail with identical results).
// 1.13c D2CMP+0x70C0
extern "C" void* d2cmp_70c0(void* dst, const void* src, size_t n) { return memmove(dst, src, n); }
