// 1.13c D2Game+0x05CA9 (_realloc): VC7.1 CRT body mapped from the retail decompile.
#include <cstdint>
#include <emscripten.h>

struct D2GameReallocEnv {
    uint32_t heap_mode; // D2Game+0x11A210, 3 selects the small-block heap
    uint32_t heap; // D2Game+0x11A20C, process heap
    uint32_t sbh_threshold; // D2Game+0x117FC0
    uint32_t newmode; // D2Game+0x10E0DC
    void* (*malloc_)(uint32_t size); // 0x05F78
    void (*free_)(void* mem); // 0x05A8D
    // The CRT's small-block heap uses 32-bit retail addresses. These two explicit adapters
    // translate only at the host boundary. The host must map this allocation's complete address
    // range to its retail 32-bit address and resolve SBH allocation results back to host memory;
    // region and block values passed among SBH callees remain retail-width addresses throughout.
    uint32_t (*to_retail_address)(const void* host_mem);
    void* (*to_host_address)(uint32_t retail_mem);
    uint32_t (*sbh_find_block)(uint32_t retail_mem); // 0x08C1D -> retail region address or 0
    int32_t (*sbh_resize_block)(uint32_t retail_region, uint32_t retail_mem, uint32_t size); // 0x0911D
    uint32_t (*sbh_alloc_block)(uint32_t size); // 0x093FC -> retail pointer
    void (*sbh_free_block)(uint32_t retail_region, uint32_t retail_mem); // 0x08C48
    void (*lock)(int32_t lock_num); // 0x0402B
    void (*unlock4)(); // 0x05E11
    void* (*heap_alloc)(uint32_t heap, uint32_t flags, uint32_t bytes);
    void* (*heap_realloc)(uint32_t heap, uint32_t flags, void* mem, uint32_t bytes);
    void* (*memcpy_)(void* dst, const void* src, uint32_t bytes); // 0x05380
    int32_t (*callnewh)(uint32_t size); // 0x09710
};

// 1.13c D2Game+0x05CA9 (417 B), called by __onexit_lk at 0x033C6. Retail's VC7.1 _realloc:
// NULL input delegates to _malloc; zero size frees; otherwise it resizes through the process heap
// or small-block heap, using 16-byte alignment for heap moves and retrying only when _newmode and
// __callnewh permit it. The callback table represents the exact retail callees for deterministic replay.
extern "C" EMSCRIPTEN_KEEPALIVE void* d2game_realloc_5ca9(
    void* mem, uint32_t size, const D2GameReallocEnv* env) {
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
            if (env->newmode == 0 || env->callnewh(size) == 0) return nullptr;
        }
    }
    for (;;) {
        void* out = nullptr;
        if (size > 0xFFFFFFE0u) {
            env->callnewh(size);
            return nullptr;
        }
        env->lock(4);
        const uint32_t retail_mem = env->to_retail_address(mem);
        uint32_t region = env->sbh_find_block(retail_mem);
        if (region != 0) {
            if (size <= env->sbh_threshold) {
                if (env->sbh_resize_block(region, retail_mem, size) != 0) {
                    out = mem;
                } else {
                    const uint32_t retail_out = env->sbh_alloc_block(size);
                    if (retail_out != 0) {
                        out = env->to_host_address(retail_out);
                    }
                    if (out != nullptr) {
                        const uint32_t old_size = *(uint32_t*)((uint8_t*)mem - 4) - 1u;
                        env->memcpy_(out, mem, size <= old_size ? size : old_size);
                        region = env->sbh_find_block(retail_mem);
                        env->sbh_free_block(region, retail_mem);
                    }
                }
            }
            if (out == nullptr) {
                size = (size + 0x0Fu) & 0xFFFFFFF0u;
                out = env->heap_alloc(env->heap, 0, size);
                if (out != nullptr) {
                    const uint32_t old_size = *(uint32_t*)((uint8_t*)mem - 4) - 1u;
                    env->memcpy_(out, mem, size <= old_size ? size : old_size);
                    env->sbh_free_block(region, retail_mem);
                }
            }
        }
        env->unlock4();
        if (region == 0) {
            size = (size + 0x0Fu) & 0xFFFFFFF0u;
            out = env->heap_realloc(env->heap, 0, mem, size);
        }
        if (out != nullptr) return out;
        if (env->newmode == 0 || env->callnewh(size) == 0) return nullptr;
    }
}
