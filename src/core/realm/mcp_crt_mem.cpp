// src/core/realm/mcp_crt_mem.cpp -- four UNMAPPED 1.13c D2MCPClient.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2mcpclient/decomp/0x<RVA>.c (alpha lane). Base 0x6FA20000; every RVA
// below is retail's.
//
// All three are MSVC 2003 (VC7.1) Release CRT memory routines the linker pulled into the module. Prior
// art: the D2Client sibling batch src/core/d2client/d2client_batch_77a1.cpp carries the byte-identical
// VC7.1 routines for that DLL (its 0x31A0 _memmove, 0x7CB8 __heap_alloc, 0x8797 calloc); the rules
// below are the same, restated against D2MCPClient's own globals. The small-block heap these functions
// prefer (0x3D23 ___sbh_alloc_block, still UNMAPPED here) and every kernel operation (critical section,
// HeapAlloc) are host/module state no argument pins, so each is a seam in D2McpCrtEnv.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2McpCrtEnv {
    uint32_t heap_mode;                       // +0x324A4 DAT_6fa324a4 (3 = small-block heap active)
    uint32_t sbh_threshold;                   // +0x32258 DAT_6fa32258, largest sbh request
    uint32_t newmode;                         // +0xAD18 DAT_6fa2ad18 (_newmode)
    int32_t (*new_handler)(uint32_t size);    // __callnewh, may be null
    void*    (*sbh_alloc_block)(uint32_t size);// 0x3D23 ___sbh_alloc_block
    void*    (*heap_alloc)(uint32_t size);    // HeapAlloc(DAT_6fa324a0, 0, size)
    void*    (*heap_alloc_zero)(uint32_t size);// HeapAlloc(DAT_6fa324a0, 8, size)
};

// 1.13c D2MCPClient+0x5260 (size 672, caller 0x356F ___sbh_free_block): the VC7.1 `_memmove`. Retail
// splits the copy into byte/word runs around pointer alignment, but that is unobservable: when the
// ranges overlap with dst above src the copy runs backwards from the tail, otherwise forwards from the
// head, and the bytes written are exactly those of memmove. Returns dst.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_mcp_memmove_5260(void* dst, const void* src, uint32_t size) {
    uint8_t* d = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;
    if (s < d && d < s + size) {                 // retail's `_Src < _Dst && _Dst < _Src + _Size`
        for (uint32_t i = size; i != 0; --i) d[i - 1] = s[i - 1];
    } else {
        for (uint32_t i = 0; i < size; ++i) d[i] = s[i];
    }
    return dst;
}

// 1.13c D2MCPClient+0x4230 (size 672, callers 0x221B and 0x29EA): VC7.1 `_memcpy`.
// The optimized retail body copies backward when dst overlaps src from above, so this is memmove
// semantics despite its CRT name. The already mapped sibling implements that byte rule.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_mcp_memcpy_4230(void* dst, const void* src, uint32_t size) {
    return d2_mcp_memmove_5260(dst, src, size);
}

// 1.13c D2MCPClient+0x02841 (size 111, caller 0x28BC __nh_malloc): the VC7.1 `__heap_alloc`. With the
// small-block heap active and the request within its threshold, take 0x3D23 under _lock(4)/_unlock(4)
// and return that block if it succeeds. Otherwise HeapAlloc(handle, 0, bytes), where a zero request is
// one byte and the size is rounded up to 16 unless the heap is in mode 1.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_mcp_heap_alloc_2841(uint32_t size, const D2McpCrtEnv* env) {
    if (env->heap_mode == 3 && size <= env->sbh_threshold) {
        void* p = env->sbh_alloc_block(size);    // _lock(4) .. _unlock(4): host state, no-op in the shim
        if (p != nullptr) return p;
    }
    uint32_t bytes = size == 0 ? 1 : size;
    if (env->heap_mode != 1) bytes = (bytes + 0x0f) & 0xfffffff0u;
    return env->heap_alloc(bytes);
}

// 1.13c D2MCPClient+0x1C25 (size 203, callers 0x11B8 __CRT_INIT@12, 0x18DE, 0x1AC5, 0x4C38, 0x5950):
// the VC7.1 `calloc`. Reject with null when count != 0 and size > 0xFFFFFFE0 / count (the product would
// not leave room for the 16-byte heap header). The product is the zeroed length; a zero product is a
// one-byte request. While the request is below 0xFFFFFFE1: in mode 3 round the request to 16 and, when
// the product is within the sbh threshold, take it from 0x3D23 under the lock and zero exactly the
// product; otherwise HeapAlloc(handle, 8, bytes). On failure, if _newmode is set ask the new-handler to
// free room and repeat the whole attempt.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_mcp_calloc_1c25(uint32_t count, uint32_t size, const D2McpCrtEnv* env) {
    if (count != 0 && size > 0xffffffe0u / count) return nullptr;
    uint32_t product = count * size;
    uint32_t bytes = product == 0 ? 1 : product;
    for (;;) {
        if (bytes < 0xffffffe1u) {
            if (env->heap_mode == 3) {
                bytes = (bytes + 0x0f) & 0xfffffff0u;
                if (product <= env->sbh_threshold) {
                    void* p = env->sbh_alloc_block(product);
                    if (p != nullptr) {
                        std::memset(p, 0, product);
                        return p;
                    }
                }
            }
            void* p = env->heap_alloc_zero(bytes);
            if (p != nullptr) return p;
        }
        if (env->newmode == 0) return nullptr;
        if (env->new_handler == nullptr || env->new_handler(bytes) == 0) return nullptr;
    }
}

// 1.13c D2MCPClient+0x4040 (96 B, callers 0x1C25 calloc, 0x4C38, 0x4E80, 0x5950): the VC7.1 `_memset`.
// A zero size returns dst untouched. Otherwise the low byte of _Val is the fill: when more than three
// bytes remain, flush dst up to its 4-byte alignment with byte stores, then store 32-bit words of the
// byte repeated (`0x01010101 * byte`) and the trailing (<4) bytes one at a time. Returns dst.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_mcp_memset_4040(void* dst, int32_t val, uint32_t size) {
    if (size == 0) return dst;
    uint8_t* d = (uint8_t*)dst;
    uint32_t fill = (uint32_t)val & 0xffu;
    if (size > 3) {
        uint32_t misalign = (uint32_t)(-(intptr_t)dst) & 3u;
        uint32_t rest = size;
        if (misalign != 0) {
            rest = size - misalign;
            do {
                *d++ = (uint8_t)val;
                misalign -= 1;
            } while (misalign != 0);
        }
        const uint32_t word = fill * 0x01010101u;
        size = rest & 3u;
        uint32_t words = rest >> 2;
        if (words != 0) {
            do {
                *(uint32_t*)d = word;
                d += 4;
                words -= 1;
            } while (words != 0);
            if (size == 0) return dst;
        }
    }
    do {
        *d++ = (uint8_t)fill;
        size -= 1;
    } while (size != 0);
    return dst;
}
