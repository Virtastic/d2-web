// src/core/d2client/d2client_realloc_7e9d.cpp -- three 1.13c D2Client.dll CRT rows reimplemented from
// the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane): 0x07E9D _realloc,
// 0x0804F __msize and the 0x0A712 _unlock(10) wrapper. Base 0x6FAB0000; every RVA below is retail's.
// The unrecovered CRT and kernel callees are host seams in retail's call order
// (src/core/d2client/d2client_free_4891.cpp and d2client_batch_77a1.cpp patterns).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// _unlock(idx), recovered in src/core/d2client/d2client_batch_3664.cpp.
extern "C" void d2_client_47ab(uint32_t idx);

struct D2ClientReallocEnv {
    void*    (*malloc_)(uint32_t size);                        // 0x7D5F _malloc(size)
    void     (*free_)(void* p);                                // 0x4891 _free(p)
    void*    (*find_block)(void* p);                           // 0x9252 ___sbh_find_block(p) -> region
    int32_t  (*resize_block)(void* region, void* p, uint32_t size);  // 0x9752 ___sbh_resize_block
    void*    (*alloc_block)(uint32_t size);                    // 0x9A31 ___sbh_alloc_block(size)
    void     (*free_block)(void* region, void* p);             // 0x927D ___sbh_free_block(region, p)
    void*    (*memcpy_)(void* dst, const void* src, uint32_t n);     // 0x5D20 _memcpy
    void     (*lock4)(void);                                   // 0x4860 _lock(4)
    void     (*unlock4)(void);                                 // 0x8005 _unlock(4)
    void*    (*heap_alloc)(uint32_t size);                     // HeapAlloc(DAT_6fbd3988, 0, size)
    void*    (*heap_realloc)(void* p, uint32_t size);          // HeapReAlloc(DAT_6fbd3988, 0, p, size)
    uint32_t (*heap_size)(void* p);                            // HeapSize(DAT_6fbd3988, 0, p)
    int32_t  (*callnewh)(uint32_t size);                       // 0xBA17 __callnewh(size)
    uint32_t heap_mode;                                        // 0x2398C DAT_6fbd398c (3 = sbh active)
    uint32_t sbh_threshold;                                    // 0x23978 DAT_6fbd3978
    uint32_t newmode;                                          // 0x0FAAC64 DAT_6fbaac64 (_newmode)
};

namespace {
constexpr uint32_t kMaxRequest = 0xFFFFFFE0u;   // above this __callnewh(size) is asked, NULL returned

inline uint32_t old_size(const void* p) {
    uint32_t word;
    std::memcpy(&word, (const uint8_t*)p - 4, 4);   // block head sentinel: size + 1
    return word - 1u;
}

inline uint32_t min32(uint32_t a, uint32_t b) { return a <= b ? a : b; }
}  // namespace

// 1.13c D2Client+0x07E9D. cdecl; [esp+4] = p, [esp+8] = size. NULL p is _malloc(size); a zero size
// _free's p and returns NULL. With the small-block heap active (mode 3) and p inside a region, resize
// in place when the request is within the sbh threshold; otherwise allocate a fresh block, copy
// min(size, old size) and free the old one. Blocks outside a region (or with the heap in another mode)
// go through HeapReAlloc. Every failure asks the new-handler (when __newmode is set) and retries.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_7e9d(void* p, uint32_t size, const D2ClientReallocEnv* env) {
    if (p == nullptr) return env->malloc_(size);
    if (size == 0) {
        env->free_(p);
        return nullptr;
    }

    if (env->heap_mode == 3) {
        for (;;) {
            void* result = nullptr;
            void* region = nullptr;
            if (size > kMaxRequest) {          // LAB_6fab8040
                env->callnewh(size);
                return nullptr;
            }
            env->lock4();
            region = env->find_block(p);
            if (region != nullptr) {
                if (size <= env->sbh_threshold) {
                    if (env->resize_block(region, p, size) == 0) {
                        result = env->alloc_block(size);
                        if (result != nullptr) {
                            env->memcpy_(result, p, min32(size, old_size(p)));
                            region = env->find_block(p);
                            env->free_block(region, p);
                        }
                    } else {
                        result = p;
                    }
                }
                if (result == nullptr) {
                    if (size == 0) size = 1;
                    size = (size + 0x0Fu) & 0xFFFFFFF0u;
                    result = env->heap_alloc(size);
                    if (result != nullptr) {
                        env->memcpy_(result, p, min32(size, old_size(p)));
                        env->free_block(region, p);
                    }
                }
            }
            env->unlock4();
            if (region == nullptr) {
                if (size == 0) size = 1;
                size = (size + 0x0Fu) & 0xFFFFFFF0u;
                result = env->heap_realloc(p, size);
            }
            if (result != nullptr) return result;
            if (env->newmode == 0) return nullptr;
            if (env->callnewh(size) == 0) break;
        }
        return nullptr;
    }

    for (;;) {
        if (size > kMaxRequest) {              // LAB_6fab8040
            env->callnewh(size);
            return nullptr;
        }
        if (size == 0) size = 1;
        void* result = env->heap_realloc(p, size);
        if (result != nullptr) return result;
        if (env->newmode == 0) return nullptr;
        if (env->callnewh(size) == 0) break;
    }
    return nullptr;
}

// 1.13c D2Client+0x0804F (__msize). With the small-block heap active, lock 4, find p in the regions
// and, inside one, return the head sentinel word (p-4) minus 9 (the stored size+1 word counts the two
// 4-byte sentinels: 8 + 1); unlock (0x80BC, the same _unlock(4)). Blocks outside a region, and any
// heap mode other than 3, ask HeapSize(DAT_6fbd3988, 0, p).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_804f(void* p, const D2ClientReallocEnv* env) {
    if (env->heap_mode == 3) {
        env->lock4();
        void* region = env->find_block(p);
        uint32_t size = 0;
        if (region != nullptr) {
            uint32_t word;
            std::memcpy(&word, (const uint8_t*)p - 4, 4);
            size = word - 9u;
        }
        env->unlock4();          // retail 0x80BC: _unlock(4)
        if (region != nullptr) return size;
    }
    return env->heap_size(p);
}

// 1.13c D2Client+0x0A712: `push 0xA; call 0x47AB` -- release CRT lock 10 (same shape as 0x4857).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_a712(void) {
    d2_client_47ab(10);
}
