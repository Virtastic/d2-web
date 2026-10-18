// src/core/realm/mcp_realloc_29ea.cpp -- one UNMAPPED 1.13c D2MCPClient.dll row reimplemented from the
// decompile in build-wasm/ghidra-d2mcpclient/decomp/0x29EA.c (alpha lane, no spec/vector yet). Retail
// base 0x6FA20000; every module global is a static named at its retail RVA.
//
// The MSVC 2003 (VC7.1) Release CRT `_realloc` the linker pulled into the module. Prior art: the
// byte-identical VC7.1 body at src/core/realm/bnclient_batch_5906.cpp 0x2275 (d2_fn_bnclient_2275,
// same 417 bytes, one DLL over) and the small-block heap it drives, already landed here at
// src/core/realm/mcp_sbh_alloc.cpp (0x3D23, 0x3A44), mcp_sbh_free.cpp (0x356F) and
// mcp_sbh_find_block.cpp (0x3544). Every kernel32 call (HeapAlloc/HeapReAlloc) and the CRT lock are
// host seams after the retail arguments, which keep their order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// The module state and callees 0x29EA reads, each named at its retail global/call RVA.
struct D2McpReallocEnv {
    uint32_t heap_mode;                      // +0x324A4 DAT_6fa324a4 (3 = small-block heap active)
    uint32_t heap;                           // +0x324A0 DAT_6fa324a0 (process heap handle)
    uint32_t sbh_threshold;                  // +0x32258 DAT_6fa32258 (largest small-block request)
    uint32_t newmode;                        // +0xAD18  DAT_6fa2ad18 (_newmode)
    void*    (*malloc_)(uint32_t size);                                    // 0x28E8 _malloc
    void     (*free_)(void* mem);                                          // 0x1BB4 _free
    uint32_t (*sbh_find_block)(void* mem);                                 // 0x3544
    int32_t  (*sbh_resize_block)(void* region, void* mem, uint32_t size);  // 0x3A44
    void*    (*sbh_alloc_block)(uint32_t size);                            // 0x3D23
    void     (*sbh_free_block)(void* region, void* mem);                   // 0x356F
    void     (*lock)(int32_t lock_num);                                    // 0x15A3 __lock
    void     (*unlock4)(void);                                             // 0x2B52 __unlock(4)
    void*    (*heap_alloc)(uint32_t heap, uint32_t flags, uint32_t bytes); // HeapAlloc
    void*    (*heap_realloc)(uint32_t heap, uint32_t flags, void* mem, uint32_t bytes); // HeapReAlloc
    void*    (*memcpy_)(void* dst, const void* src, uint32_t n);           // 0x4230 _memcpy
    int32_t  (*callnewh)(uint32_t size);                                   // 0x401F __callnewh
};

// 1.13c D2MCPClient+0x029EA (417 B, caller 0x15D4): _realloc(mem, size). NULL mem is malloc(size), zero
// size frees and returns NULL. In small-block mode (heap_mode 3) each attempt first rejects a request
// above 0xFFFFFFE0 through __callnewh, then takes __lock(4), looks the block up, and either resizes in
// place, moves it to a fresh small block (copying min(size, old header size - 1) bytes) or to a 16-byte
// rounded HeapAlloc block, frees the old block, and always unlocks; a block outside the small-block
// heap goes straight to a rounded HeapReAlloc. In normal mode each attempt is HeapReAlloc(size) unless
// size exceeds 0xFFFFFFE0. A null result ends the call when _newmode is 0, otherwise it retries only
// while __callnewh reports a handler ran.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_mcp_realloc_29ea(void* mem, uint32_t size, const D2McpReallocEnv* env) {
    if (mem == nullptr) return env->malloc_(size);
    if (size == 0) {
        env->free_(mem);
        return nullptr;
    }
    if (env->heap_mode != 3) {
        for (;;) {
            if (size > 0xFFFFFFE0u) {
                env->callnewh(size);
                return nullptr;
            }
            void* const out = env->heap_realloc(env->heap, 0, mem, size);
            if (out != nullptr) return out;
            if (env->newmode == 0) return nullptr;
            if (env->callnewh(size) == 0) return nullptr;
        }
    }
    for (;;) {
        void* out = nullptr;
        if (size > 0xFFFFFFE0u) {
            env->callnewh(size);
            return nullptr;
        }
        env->lock(4);
        void* region = (void*)(uintptr_t)env->sbh_find_block(mem);
        if (region != nullptr) {
            if (size <= env->sbh_threshold) {
                if (env->sbh_resize_block(region, mem, size) != 0) {
                    out = mem;
                } else {
                    out = env->sbh_alloc_block(size);
                    if (out != nullptr) {
                        const uint32_t old = *(uint32_t*)((uint8_t*)mem - 4) - 1u;
                        env->memcpy_(out, mem, size <= old ? size : old);
                        region = (void*)(uintptr_t)env->sbh_find_block(mem);
                        env->sbh_free_block(region, mem);
                    }
                }
            }
            if (out == nullptr) {
                size = (size + 0x0Fu) & 0xFFFFFFF0u;
                out = env->heap_alloc(env->heap, 0, size);
                if (out != nullptr) {
                    const uint32_t old = *(uint32_t*)((uint8_t*)mem - 4) - 1u;
                    env->memcpy_(out, mem, size <= old ? size : old);
                    env->sbh_free_block(region, mem);
                }
            }
        }
        env->unlock4();
        if (region == nullptr) {
            size = (size + 0x0Fu) & 0xFFFFFFF0u;
            out = env->heap_realloc(env->heap, 0, mem, size);
        }
        if (out != nullptr) return out;
        if (env->newmode == 0) return nullptr;
        if (env->callnewh(size) == 0) return nullptr;
    }
}
