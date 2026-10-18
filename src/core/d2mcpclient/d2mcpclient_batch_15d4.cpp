// src/core/d2mcpclient/d2mcpclient_batch_15d4.cpp -- one UNMAPPED 1.13c D2MCPClient.dll row reimplemented
// from build-wasm/ghidra-d2mcpclient/decomp/0x15D4.c (alpha lane). Base 0x6FA20000; the two table globals
// and both calls are named at their retail RVAs. The row is MSVC 2003 (VC7.1) Release `__onexit_lk`, the
// append half of the CRT onexit table (`__onexit` at 0x167C is its only caller and takes `_lockexit`).
// `_msize` (0x2B9C) and `_realloc` (0x29EA) are seams; the table pointers live in the env so the rule stays
// a pure function of the state it is handed.
#include <cstdint>
#include <emscripten.h>

struct D2McpOnexitLkEnv {
    uint32_t (*msize)(void* p);                      // 0x2B9C __msize: usable size of a heap block
    uint32_t* (*realloc)(uint32_t* p, uint32_t size); // 0x29EA CRT realloc, NULL on failure
    uint32_t* onexit_begin;                          // +0x125D0 DAT_6fa325d0, table base
    uint32_t* onexit_end;                            // +0x125CC DAT_6fa325cc, next free slot
};

// 1.13c D2MCPClient+0x015D4
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_onexit_lk(uint32_t func, D2McpOnexitLkEnv* env) {
    const uint32_t size = env->msize(env->onexit_begin);
    // bytes used + 4 (one more slot) > block size: grow.
    if (size < (uint32_t)(int32_t)((intptr_t)env->onexit_end + (4 - (intptr_t)env->onexit_begin))) {
        uint32_t grow = 0x800;
        if (size < 0x800) grow = size;
        uint32_t* block = env->realloc(env->onexit_begin, grow + size);
        if (block == nullptr) {
            block = env->realloc(env->onexit_begin, size + 0x10);
            if (block == nullptr) return;
        }
        env->onexit_end = block +
            (((int32_t)(intptr_t)env->onexit_end - (int32_t)(intptr_t)env->onexit_begin) >> 2);
        env->onexit_begin = block;
    }
    *env->onexit_end = func;
    env->onexit_end += 1;
}
