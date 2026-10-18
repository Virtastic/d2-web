// src/core/d2game/d2game_crt_calloc.cpp -- 1.13c D2Game.dll row 0x06018 __calloc_crt, from the cached
// decompile (build-wasm/ghidra-d2game/decomp/0x6018.c). Alpha batch lane (OBJECTIVE v8.3): implementation
// + marker only, no spec, no vector.
//
// Host model follows src/core/d2game/d2game_crt_lseek.cpp: retail module globals are the statics below at
// their retail RVA; callees whose own rows are still UNMAPPED are seams on a caller-supplied struct in
// retail argument order. The __SEH_prolog/__SEH_epilog callees (0x402B / 0x411F) bracket the body only so
// the lock is released on both paths; the model releases it directly on every return.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// The callees this row's decompile calls and whose rows are not mapped yet (RVA = retail address).
struct D2GameCrtCallocEnv {
    void     (*lock)(int32_t locknum);                        // 0x40E4 __lock
    void     (*unlock)(int32_t locknum);                      // 0x411F __unlock
    int32_t* (*sbhAllocBlock)(uint32_t size);                 // 0x93FC ___sbh_alloc_block
    void     (*sub60DE)(void);                                // 0x60DE small-block heap bookkeeping
    void*    (*heapAlloc)(uint32_t bytes);                    // HeapAlloc(0x6FD3A20C, 8, bytes)
    int32_t  (*callnewh)(uint32_t size);                      // 0x9710 __callnewh
};

// 0x6FD3A210: allocator mode; 3 selects the small-block heap.
static int32_t  g_allocMode = 0;
// 0x6FD39FC0: largest request the small-block heap serves.
static uint32_t g_sbhThreshold = 0;
// 0x6FD300DC: non-zero when a _new_handler is installed.
static int32_t  g_hasNewHandler = 0;

// 1.13c D2Game+0x06018 (__calloc_crt): void* __cdecl calloc(size_t count, size_t size). count == 0 or
// size <= 0xFFFFFFE0/count (the unsigned overflow guard; count == 0 short-circuits the division) yields
// bytes = count*size, and a zero byte count grows to one for the allocator. Each pass: with the mode 3
// small-block heap, a 16-byte-rounded request and bytes <= threshold, the block lock (4) is taken,
// ___sbh_alloc_block(bytes) tried and 0x60DE run; success memsets `bytes` and returns. Otherwise (or on
// failure) HeapAlloc(heap, 8, rounded) is tried. A failure returns null unless a new handler is installed,
// in which case 0x9710(rounded) is called and the pass repeats while it returns non-zero. The overflow
// case returns null without allocating.
// 1.13c D2Game+0x06018
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2game_6018(uint32_t count, uint32_t size,
                                             const D2GameCrtCallocEnv* env) {
    if (count != 0 && size > 0xFFFFFFE0u / count) return nullptr;   // overflow
    const uint32_t bytes = count * size;
    uint32_t dwBytes = bytes != 0 ? bytes : 1;
    for (;;) {
        if (dwBytes < 0xFFFFFFE1u) {
            void* dst = nullptr;
            if (g_allocMode == 3) {
                dwBytes = (dwBytes + 0xFu) & 0xFFFFFFF0u;
                if (bytes <= g_sbhThreshold) {
                    env->lock(4);
                    dst = env->sbhAllocBlock(bytes);
                    env->sub60DE();
                    if (dst != nullptr) {
                        std::memset(dst, 0, bytes);
                        env->unlock(4);
                        return dst;
                    }
                    env->unlock(4);
                }
            }
            dst = env->heapAlloc(dwBytes);
            if (dst != nullptr) return dst;
        } else {
            void* dst = env->heapAlloc(dwBytes);
            if (dst != nullptr) return dst;
        }
        if (g_hasNewHandler == 0) return nullptr;
        if (env->callnewh(dwBytes) == 0) return nullptr;
    }
}
