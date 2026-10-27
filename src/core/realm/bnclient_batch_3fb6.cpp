// src/core/realm/bnclient_batch_3fb6.cpp -- one UNMAPPED 1.13c BNClient.dll row reimplemented from the
// decompile build-wasm/ghidra-bnclient/decomp/0x3FB6.c (alpha lane). Base 0x6FF20000; every global is
// named at its retail RVA. This is MSVC 2003 (VC7.1) Release `calloc`; the byte-identical siblings are
// src/core/realm/mcp_crt_mem.cpp 0x1C25 (D2MCPClient) and src/core/d2client/d2client_batch_77a1.cpp
// 0x8797 (D2Client). The small-block allocator 0x51F0, both lock leaves (0x3163 __lock, 0x407C
// __unlock(4)) and the new-handler 0x49AE are UNMAPPED, and HeapAlloc is a kernel import: each is a
// seam after the retail arguments. _memset (0x6C00) is std::memset, which writes the same bytes.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct BnCallocEnv {
    uint32_t heap_mode;                        // DAT_6FF40BE4 __active_heap (3 = small-block heap)
    uint32_t sbh_threshold;                    // DAT_6FF40BD0 largest request the sbh serves
    uint32_t newmode;                          // DAT_6FF3E604 __newmode
    int32_t (*new_handler)(uint32_t size);     // 0x49AE __callnewh; null when no handler is installed
    void*    (*sbh_alloc_block)(uint32_t size); // 0x51F0 ___sbh_alloc_block
    void     (*lock4)(void);                   // 0x3163 __lock(4)
    void     (*unlock4)(void);                 // 0x407C __unlock(4) thunk, caller of 0x30AE
    void*    (*heap_alloc_zero)(uint32_t size); // HeapAlloc(DAT_6FF40BE0, 8, size)
};

// 1.13c BNClient+0x03FB6 (203 B, callers 0x1F1F/0x1FDD/0x3255/0x343C/0x6D18/0x6FA7): the VC7.1
// `calloc`. Reject with null when count != 0 and size > 0xFFFFFFE0 / count (the product must leave
// room for the 16-byte heap header). The product is the zeroed length; a zero product is a one-byte
// request. While the request is below 0xFFFFFFE1: in heap mode 3 round it up to 16 and, when the
// product is within the sbh threshold, take it from 0x51F0 under lock 4, zero exactly the product and
// return it; otherwise HeapAlloc(handle, 8, bytes). On failure, if __newmode is set ask the new-handler
// to free room (0x49AE returns 0 when it cannot) and repeat the whole attempt; otherwise return null.
extern "C"
EMSCRIPTEN_KEEPALIVE void* bn_calloc_3fb6(uint32_t count, uint32_t size, const BnCallocEnv* env) {
    if (count != 0 && size > 0xffffffe0u / count) return nullptr;
    uint32_t product = count * size;
    uint32_t bytes = product == 0 ? 1 : product;
    for (;;) {
        if (bytes < 0xffffffe1u) {
            if (env->heap_mode == 3) {
                bytes = (bytes + 0x0fu) & 0xfffffff0u;
                if (product <= env->sbh_threshold) {
                    env->lock4();
                    void* p = env->sbh_alloc_block(product);
                    env->unlock4();
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
