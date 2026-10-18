// src/core/fog/fog_batch_1fad.cpp -- twelve UNMAPPED 1.13c Fog.dll rows transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-fog/decomp/0x<RVA>.c), in RVA order. Alpha batch lane
// (OBJECTIVE v8.3): implementation + marker only, no spec, no vector. Retail base 0x6FF50000.
//
// All twelve are MSVC 2003 (VC7.1) Release CRT bodies the linker pulled into Fog.dll: the heap
// (_free, __heap_alloc, __nh_malloc, _malloc, _realloc), three C-locale classifiers (_isxdigit,
// _isspace, _isalnum), the two formatted-string front ends (_sscanf, __snprintf) and the onexit
// table pair (__onexit_lk, ___onexitinit).
//
// Host model follows src/core/realm/bnclient_batch_5906.cpp and bnclient_batch_1000.cpp: kernel
// heap calls and the small-block-heap helpers are seams on FogHeapEnv (retail global RVA named at
// each field), the CRT lock/unlock leaves are the already-implemented Fog +0x2000/+0x2090/+0x223F
// _unlock(4) no-ops in fog_batch_2000.cpp, and the host formatter stands in for the retail stdio
// engines +0x55863 / +0x411F. Ghidra's `extraout_ECX`/register-only `unaff_EDI` arguments are made
// explicit parameters.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdarg>

#include <emscripten.h>

// The CRT heap state these rows read. Every pointer is a seam; the caller wires the module globals
// the retail image uses.
struct FogHeapEnv {
    uint32_t heap_mode;      // +0x5A284 DAT_6ffa8284 (3 = small-block heap active)
    uint32_t heap;           // +0x5A280 DAT_6ffa8280 (process heap handle)
    uint32_t sbh_threshold;  // +0x5A270 DAT_6ffa8270 (largest small-block request)
    uint32_t newmode;        // +0x303D4 DAT_6ff803d4 (_newmode: the __nh_malloc retry flag)
    void    (*lock)(int32_t lock_num);                                          // __lock
    void*   (*sbh_find_block)(void* mem);                                       // ___sbh_find_block
    int32_t (*sbh_resize_block)(void* region, void* mem, uint32_t size);        // ___sbh_resize_block
    void*   (*sbh_alloc_block)(uint32_t size);                                  // ___sbh_alloc_block
    void    (*sbh_free_block)(void* region, void* mem);                         // ___sbh_free_block
    int32_t (*callnewh)(uint32_t size);                                         // __callnewh
    void*   (*heap_alloc)(uint32_t heap, uint32_t flags, uint32_t bytes);       // HeapAlloc
    void*   (*heap_realloc)(uint32_t heap, uint32_t flags, void* mem, uint32_t bytes); // HeapReAlloc
    int32_t (*heap_free)(uint32_t heap, uint32_t flags, void* mem);             // HeapFree
    size_t  (*msize)(void* mem);                                                // __msize
};

// The onexit table: +0x5A298 base (DAT_6ffa8298) and +0x5A294 tail (DAT_6ffa8294), file state as in
// retail (the caller owns the block once it is allocated).
static uintptr_t g_fog_onexit_base;
static uintptr_t g_fog_onexit_tail;

// +0x2000 / +0x2090 / +0x223F: the three nine-byte _unlock(4) leaves, implemented in
// fog_batch_2000.cpp; each is called at the point retail calls it.
extern "C" void fog_sub_2000(void);
extern "C" void fog_sub_2090(void);
extern "C" void fog_sub_223F(void);

// 1.13c Fog+0x1FAD
// _free(mem): NULL is a no-op. In small-block mode (heap_mode 3) take __lock(4), look the block up;
// if it belongs to the small-block heap, free it there, then __unlock(4) (0x2000) and return. A block
// outside that heap (or a miss) falls through to HeapFree(heap, 0, mem); the lookup must not have
// freed anything in that case, exactly as the decompile orders it.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_free(void* mem, const FogHeapEnv* env) {  // 1.13c Fog+0x1FAD
    if (mem == nullptr) return;
    if (env->heap_mode == 3) {
        env->lock(4);
        void* const region = env->sbh_find_block(mem);
        if (region != nullptr) env->sbh_free_block(region, mem);
        fog_sub_2000();
        if (region != nullptr) return;
    }
    env->heap_free(env->heap, 0, mem);
}

// 1.13c Fog+0x201E
// __heap_alloc(size): a request no larger than the small-block threshold and with the small-block
// heap active goes to ___sbh_alloc_block under __lock(4), then __unlock(4) (0x2090); a hit returns
// it. Otherwise a zero request becomes 1, anything but heap_mode 1 is rounded up to a 16-byte
// multiple, and the block comes from HeapAlloc.
extern "C" EMSCRIPTEN_KEEPALIVE void* fog_heap_alloc(size_t size, const FogHeapEnv* env) {  // 1.13c Fog+0x201E
    if (env->heap_mode == 3 && size <= env->sbh_threshold) {
        env->lock(4);
        void* const block = env->sbh_alloc_block((uint32_t)size);
        fog_sub_2090();
        if (block != nullptr) return block;
    }
    if (size == 0) size = 1;
    if (env->heap_mode != 1) size = (size + 0xFu) & ~(size_t)0xFu;
    return env->heap_alloc(env->heap, 0, (uint32_t)size);
}

// 1.13c Fog+0x2099
// __nh_malloc(size, nh_flag): requests at or above 0xFFFFFFE1 return NULL without trying. Each
// attempt is __heap_alloc; a NULL result ends the call when nh_flag is 0, otherwise __callnewh(size)
// decides whether to retry.
extern "C" EMSCRIPTEN_KEEPALIVE void* fog_nh_malloc(size_t size, int nh_flag, const FogHeapEnv* env) {  // 1.13c Fog+0x2099
    if (size < 0xFFFFFFE1u) {
        for (;;) {
            void* const block = fog_heap_alloc(size, env);
            if (block != nullptr) return block;
            if (nh_flag == 0) return nullptr;
            if (env->callnewh((uint32_t)size) == 0) return nullptr;
        }
    }
    return nullptr;
}

// 1.13c Fog+0x20C5
// _malloc(size): __nh_malloc(size, _newmode).
extern "C" EMSCRIPTEN_KEEPALIVE void* fog_malloc(size_t size, const FogHeapEnv* env) {  // 1.13c Fog+0x20C5
    return fog_nh_malloc(size, (int)env->newmode, env);
}

// 1.13c Fog+0x20D7
// _realloc(mem, size): NULL mem is malloc(size); size 0 frees and returns NULL. Small-block mode
// (heap_mode 3) retries: a request above 0xFFFFFFE0 goes to __callnewh and returns NULL; take
// __lock(4) and look the block up. Inside the small-block heap and within the threshold, resize in
// place, else move to a fresh small block (copying min(size, old header size - 1) bytes) and free the
// old one; an unresized block moves to a rounded HeapAlloc block the same way. A block outside that
// heap goes straight to a rounded HeapReAlloc. __unlock(4) (0x223F) then runs. A null result ends the
// call when _newmode is 0, otherwise retries only while __callnewh reports a handler ran.
extern "C" EMSCRIPTEN_KEEPALIVE void* fog_realloc(void* mem, uint32_t size, const FogHeapEnv* env) {  // 1.13c Fog+0x20D7
    if (mem == nullptr) return fog_malloc(size, env);
    if (size == 0) {
        fog_free(mem, env);
        return nullptr;
    }
    if (env->heap_mode != 3) {
        for (;;) {
            if (size > 0xFFFFFFE0u) {
                env->callnewh(size);
                return nullptr;
            }
            if (size == 0) size = 1;
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
        void* region = env->sbh_find_block(mem);
        if (region != nullptr) {
            if (size <= env->sbh_threshold) {
                if (env->sbh_resize_block(region, mem, size) != 0) {
                    out = mem;
                } else {
                    out = env->sbh_alloc_block(size);
                    if (out != nullptr) {
                        const uint32_t old = *(uint32_t*)((uint8_t*)mem - 4) - 1u;
                        std::memcpy(out, mem, size <= old ? size : old);
                        region = env->sbh_find_block(mem);
                        env->sbh_free_block(region, mem);
                    }
                }
            }
            if (out == nullptr) {
                if (size == 0) size = 1;
                size = (size + 0xFu) & 0xFFFFFFF0u;
                out = env->heap_alloc(env->heap, 0, size);
                if (out != nullptr) {
                    const uint32_t old = *(uint32_t*)((uint8_t*)mem - 4) - 1u;
                    std::memcpy(out, mem, size <= old ? size : old);
                    env->sbh_free_block(region, mem);
                }
            }
        }
        fog_sub_223F();
        if (region == nullptr) {
            if (size == 0) size = 1;
            size = (size + 0xFu) & 0xFFFFFFF0u;
            out = env->heap_realloc(env->heap, 0, mem, size);
        }
        if (out != nullptr) return out;
        if (env->newmode == 0) return nullptr;
        if (env->callnewh(size) == 0) return nullptr;
    }
}

// 1.13c Fog+0x2302
// _isxdigit(c): the process default locale is the C locale (PTR_DAT_6ff7e0d4), so the category-1
// refcount is 1 and the single-byte lc_ctype table branch runs: table[c] & 0x80. ASCII '0'..'9',
// 'A'..'F', 'a'..'f' carry 0x80; everything else (including EOF, whose entry precedes the table) is 0.
extern "C" EMSCRIPTEN_KEEPALIVE int fog_isxdigit(int c) {  // 1.13c Fog+0x2302
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) return 0x80;
    return 0;
}

// 1.13c Fog+0x2341
// _isspace(c): same C-locale single-byte table branch, mask 8. Space, \t, \n, \v, \f and \r carry it.
extern "C" EMSCRIPTEN_KEEPALIVE int fog_isspace(int c) {  // 1.13c Fog+0x2341
    return (c == ' ' || (c >= '\t' && c <= '\r')) ? 8 : 0;
}

// 1.13c Fog+0x237B
// _isalnum(c): same branch, 16-bit table read masked with 0x107: 'A'..'Z' return 0x101, 'a'..'z'
// 0x102, '0'..'9' 0x04, anything else 0.
extern "C" EMSCRIPTEN_KEEPALIVE int fog_isalnum(int c) {  // 1.13c Fog+0x237B
    if (c >= 'A' && c <= 'Z') return 0x101;
    if (c >= 'a' && c <= 'z') return 0x102;
    if (c >= '0' && c <= '9') return 0x04;
    return 0;
}

// 1.13c Fog+0x23BA
// _sscanf(src, fmt, ...): builds the read-only FILE view {_flag = 0x49, _base = _ptr = src,
// _cnt = strlen(src)} and runs the input engine at 0x55863 over the stack arguments, returning the
// number of fields converted. The host vsscanf stands in for 0x55863.
extern "C" EMSCRIPTEN_KEEPALIVE int fog_sscanf(const char* src, const char* fmt, ...) {  // 1.13c Fog+0x23BA
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsscanf(src, fmt, ap);
    va_end(ap);
    return n;
}

// 1.13c Fog+0x23EE
// __snprintf(dest, count, fmt, ...): the mirror FILE {_cnt = count, _flag = 0x42,
// _base = _ptr = dest} and the format engine at 0x411F; then, when dest is not NULL, consume one byte
// of the budget and either __flsbuf(0, &file) (budget exhausted, no room for the terminator) or write
// the NUL. The host vsnprintf stands in for 0x411F and terminates within count; its return value is
// the converted count.
extern "C" EMSCRIPTEN_KEEPALIVE int fog_snprintf(char* dest, size_t count, const char* fmt, ...) {  // 1.13c Fog+0x23EE
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsnprintf(dest, count, fmt, ap);
    va_end(ap);
    return n;
}

// 1.13c Fog+0x2445
// __onexit_lk(func): append the function pointer retail passes in EDI to the onexit table. The block
// size comes from __msize(base). When the table lacks room for one 4-byte slot
// (size < tail - base + 4), grow by min(size, 0x800) through _realloc and rebase the tail to
// new_base + (tail - base); a failed grow retries once with size + 0x10, and a second failure returns
// without storing. Otherwise store the pointer at the tail and advance the tail by 4.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_onexit_lk(uint32_t func, const FogHeapEnv* env) {  // 1.13c Fog+0x2445
    const uintptr_t base = g_fog_onexit_base;
    const uintptr_t tail = g_fog_onexit_tail;
    const size_t size = env->msize((void*)base);
    if (size < (size_t)(tail + 4 - base)) {
        const size_t grow = size < 0x800 ? size : 0x800;
        void* grown = fog_realloc((void*)base, (uint32_t)(grow + size), env);
        if (grown == nullptr) {
            grown = fog_realloc((void*)base, (uint32_t)(size + 0x10), env);
            if (grown == nullptr) return;
        }
        g_fog_onexit_tail = (uintptr_t)grown + (tail - base);
        g_fog_onexit_base = (uintptr_t)grown;
    }
    *(uint32_t*)g_fog_onexit_tail = func;
    g_fog_onexit_tail += 4;
}

// 1.13c Fog+0x24C5
// ___onexitinit(): _malloc(0x80); NULL returns 0x18 (ENOMEM) and leaves the globals untouched.
// Otherwise the first dword is zeroed (NULL terminator), base = tail = the block, and the result is 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_onexitinit(const FogHeapEnv* env) {  // 1.13c Fog+0x24C5
    void* const table = fog_malloc(0x80, env);
    if (table == nullptr) return 0x18;
    g_fog_onexit_base = (uintptr_t)table;
    *(uint32_t*)table = 0;
    g_fog_onexit_tail = g_fog_onexit_base;
    return 0;
}
