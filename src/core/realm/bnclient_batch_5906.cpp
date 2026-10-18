// src/core/realm/bnclient_batch_5906.cpp -- twelve UNMAPPED 1.13c BNClient.dll alpha-lane rows (Stage A:
// implementation + marker only, no spec, no vector), transcribed from the cached decompiles in
// build-wasm/ghidra-bnclient/decomp/0x<RVA>.c (image base 0x6FF20000). These are the MSVC 2003 (VC7.1)
// Release CRT leaves around process/thread start-up and the small-block heap. Module state the decompile
// reads through globals (the heap handle and mode, the new-handler mode, the lock table and its static
// critical sections, the C-initializer tables) arrives as explicit seam arguments after the retail
// arguments, in retail order; kernel/CRT seams are function pointers.
#include <Windows.h>   // CRITICAL_SECTION / InitializeCriticalSection: host state, no-op under wasm
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

extern "C" void bn_unlock_lock(int32_t lock_num);   // shared slot release, bnclient_batch_2558.cpp

// +0x3D634/D638/D63C DAT_6ff3d634/38/3c: the __NLG notifier's saved EAX, stack argument and EBP.
static uint32_t g_bn_nlg_eax, g_bn_nlg_param, g_bn_nlg_ebp;

// 1.13c BNClient+0x05906 (24 B, callers 0x263A/0x5872): __NLG_Notify, the thunk __local_unwind2 calls
// before a filterless handler -- store the stack argument at +0x3D638, EAX at +0x3D634 and EBP at
// +0x3D63C for the notifier. EAX/EBP cannot ride a C signature, so they are the seam after the retail
// stack argument. Same body as D2Gfx 0x39B6 (src/core/d2gfx/d2gfx_batch_b45.cpp).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_5906(uint32_t param, uint32_t eax, uint32_t ebp) {
    g_bn_nlg_param = param;
    g_bn_nlg_eax = eax;
    g_bn_nlg_ebp = ebp;
}

// 1.13c BNClient+0x06255 (16 B, caller 0x6265 ___crtInitCritSecAndSpinCount when kernel32 has no
// InitializeCriticalSectionAndSpinCount): ___crtInitCritSecNoSpinCount@8 -- initialise the critical
// section and report 1. The stdcall @8 spin count at [esp+8] is never read. Same body as D2Client
// 0x908F's fallback and D2Gfx 0x37BD.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_bnclient_6255(void* cs, uint32_t spin) {
    (void)spin;
    InitializeCriticalSection((LPCRITICAL_SECTION)cs);
    return 1;
}

// 1.13c BNClient+0x0250F (9 B, caller 0x249D): __unlock(4) -- leave the CRT lock slot 4. A second copy
// of 0x23DD/0x2494, one row each.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_250f(void) {
    bn_unlock_lock(4);
}

typedef void* (*BnOnexitFn)(void* func);

// 1.13c BNClient+0x0137A (18 B, caller 0x143C __cinit): _atexit. Register the function through 0x1342
// __onexit; a non-null result (the registered pointer) means success, so return 0, and a null result
// returns -1. The `(p != 0) - 1` form in the decompile is exactly that.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_bnclient_137a(void* func, BnOnexitFn onexit) {
    const void* const registered = onexit(func);
    return (registered != nullptr) - 1;
}

struct BnCinitEnv {
    void    (*dynamic_init)(int32_t);        // DAT_6FF40D20: C dynamic-initializer hook
    int32_t (*const* xi)(void);              // 0x6FF3D024: C-initializer table (5 slots)
    uint32_t xi_count;
    int32_t (*atexit_)(void* func);          // 0x137A _atexit
    void*    exit_thunk;                     // LAB_6FF231D8, registered before the C++ walk
    int32_t (*const* xc)(void);              // 0x6FF3D000: C++-initializer table (8 slots)
    uint32_t xc_count;
};

// 1.13c BNClient+0x0143C (106 B, caller 0x1FDD __CRT_INIT@12): __cinit(param). Run the dynamic-init hook
// with param when present. Walk the C-initializer table from +0x3D024 to +0x3D038 (five pointers),
// stopping at the first nonzero status: each iteration checks the status before calling the next non-null
// slot. When every slot succeeded, register the exit thunk with _atexit, then call every non-null slot of
// the C++-initializer table +0x3D000..+0x3D020 (eight pointers) and return 0. Same rule as D2MCPClient
// 0x105A (src/core/realm/mcp_exit.cpp).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_bnclient_143c(int32_t param, const BnCinitEnv* env) {
    if (env->dynamic_init != nullptr) env->dynamic_init(param);
    int32_t status = 0;
    for (uint32_t i = 0; i < env->xi_count; ++i) {
        if (status != 0) return status;
        if (env->xi[i] != nullptr) status = env->xi[i]();
    }
    if (status == 0) {
        env->atexit_(env->exit_thunk);
        for (uint32_t i = 0; i < env->xc_count; ++i)
            if (env->xc[i] != nullptr) (void)env->xc[i]();
        status = 0;
    }
    return status;
}

// The CRT heap the realloc/msize rows below read. Every pointer is a seam; the two rows keep the retail
// argument order and let the caller wire the same module globals the retail image uses.
struct BnHeapEnv {
    uint32_t heap_mode;                      // +0x40BE4 DAT_6FF40BE4 (3 = small-block heap active)
    uint32_t heap;                           // +0x40BE0 DAT_6FF40BE0 (process heap handle)
    uint32_t sbh_threshold;                  // +0x40BD0 DAT_6FF40BD0 (largest small-block request)
    uint32_t newmode;                        // +0x3E604 DAT_6FF3E604 (_newmode)
    void*    (*sbh_find_block)(void* mem);                       // 0x4A11 ___sbh_find_block
    int32_t  (*sbh_resize_block)(void* region, void* mem, uint32_t size);   // 0x4F11
    void*    (*sbh_alloc_block)(uint32_t size);                  // 0x51F0 ___sbh_alloc_block
    void     (*sbh_free_block)(void* region, void* mem);         // 0x4A3C ___sbh_free_block
    void     (*lock)(int32_t lock_num);                          // 0x3163 __lock
    void     (*unlock)(int32_t lock_num);                        // 0x30AE __unlock
    void*    (*heap_alloc)(uint32_t heap, uint32_t flags, uint32_t bytes);          // HeapAlloc
    void*    (*heap_realloc)(uint32_t heap, uint32_t flags, void* mem, uint32_t bytes); // HeapReAlloc
    uint32_t (*heap_size)(uint32_t heap, uint32_t flags, void* mem);               // HeapSize
    void*    (*memcpy_)(void* dst, const void* src, uint32_t n); // 0x54F0 _memcpy
    int32_t  (*callnewh)(uint32_t size);                         // 0x49AE __callnewh
    void*    (*malloc_)(uint32_t size);                          // 0x2544 _malloc
    void     (*free_)(void* mem);                                // 0x3CF4 _free
};

// 1.13c BNClient+0x02275 (417 B, caller 0x129A __onexit): _realloc(mem, size). NULL mem is malloc(size),
// zero size frees and returns NULL. In small-block mode (heap_mode 3) each attempt rounds a huge request
// to __callnewh+NULL first, takes __lock(4), looks the block up, and either resizes in place, moves it to
// a fresh small block (copying min(size, old block size - 1) bytes) or to a rounded HeapAlloc block,
// frees the old block, __unlock(4); a block outside the small-block heap goes straight to a rounded
// HeapReAlloc. In normal mode each attempt is HeapReAlloc(size) unless size exceeds 0xFFFFFFE0. A null
// result either ends the call (_newmode 0) or retries only while __callnewh reports a handler ran.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_bnclient_2275(void* mem, uint32_t size, const BnHeapEnv* env) {
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
        void* region = env->sbh_find_block(mem);
        if (region != nullptr) {
            if (size <= env->sbh_threshold) {
                if (env->sbh_resize_block(region, mem, size) != 0) {
                    out = mem;
                } else {
                    out = env->sbh_alloc_block(size);
                    if (out != nullptr) {
                        const uint32_t old = *(uint32_t*)((uint8_t*)mem - 4) - 1u;
                        env->memcpy_(out, mem, size <= old ? size : old);
                        region = env->sbh_find_block(mem);
                        env->sbh_free_block(region, mem);
                    }
                }
            }
            if (out == nullptr) {
                size = (size + 0xFu) & 0xFFFFFFF0u;
                out = env->heap_alloc(env->heap, 0, size);
                if (out != nullptr) {
                    const uint32_t old = *(uint32_t*)((uint8_t*)mem - 4) - 1u;
                    env->memcpy_(out, mem, size <= old ? size : old);
                    env->sbh_free_block(region, mem);
                }
            }
        }
        env->unlock(4);
        if (region == nullptr) {
            size = (size + 0xFu) & 0xFFFFFFF0u;
            out = env->heap_realloc(env->heap, 0, mem, size);
        }
        if (out != nullptr) return out;
        if (env->newmode == 0) return nullptr;
        if (env->callnewh(size) == 0) return nullptr;
    }
}

// 1.13c BNClient+0x02427 (106 B, caller 0x129A __onexit): __msize(mem). In small-block mode take
// __lock(4), look the block up, read its header word at mem-4 minus 9 when found, then __unlock(4) (the
// 0x2494 wrapper); a found block returns that size. Anything else (or a small-block miss) is
// HeapSize(heap, 0, mem).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_bnclient_2427(void* mem, const BnHeapEnv* env) {
    if (env->heap_mode == 3) {
        env->lock(4);
        const uint32_t block = (uint32_t)(uintptr_t)env->sbh_find_block(mem);
        const int32_t size = *(int32_t*)((uint8_t*)mem - 4) - 9;
        env->unlock(4);
        if (block != 0) return (uint32_t)size;
    }
    return env->heap_size(env->heap, 0, mem);
}

struct BnMtInitLocksEnv {
    uint32_t* lock_table;                    // +0x3D090 DAT_6ff3d090, 8-byte entries
    uint32_t  static_sections;               // +0x3E380 DAT_6ff3e380, 0x18-byte critical sections
    int32_t (*init_crit_sec)(void* cs, uint32_t spin);   // 0x6265 ___crtInitCritSecAndSpinCount
};

// 1.13c BNClient+0x03010 (73 B, caller 0x343C __mtinit): __mtinitlocks. For each of the 0x24 lock-table
// entries whose flag word (+4) is 1, install the i-th 0x18-byte static critical section from +0x3E380,
// initialise it with spin count 0xFA0, and on failure clear the pointer and return 0. Entries with any
// other flag are left alone. Returns 1 when every static lock is up.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_bnclient_3010(const BnMtInitLocksEnv* env) {
    for (uint32_t i = 0; i < 0x24; ++i) {
        uint32_t* const slot = env->lock_table + i * 2;
        if (slot[1] == 1) {
            *slot = env->static_sections + i * 0x18u;
            if (env->init_crit_sec((void*)(uintptr_t)*slot, 0xFA0) == 0) {
                *slot = 0;
                return 0;
            }
        }
    }
    return 1;
}

struct BnMtLocksEnv {
    uint32_t* lock_table;                    // +0x3D090 DAT_6ff3d090, 8-byte entries
    uint32_t  entry_count;                   // retail end 0x6FF3D1B0 - base => 0x24 entries
    void     (*delete_critical_section)(void* cs);
    void     (*free_)(void* mem);            // 0x3CF4 _free
};

// 1.13c BNClient+0x03059 (85 B, caller 0x3225 __mtterm): __mtdeletelocks. First pass over the lock
// table: every live entry whose flag is not 1 is a heap critical section -- delete it, free it, clear the
// slot. Second pass: delete (but do not free) every live entry whose flag is 1, the static sections.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_3059(const BnMtLocksEnv* env) {
    for (uint32_t i = 0; i < env->entry_count; ++i) {
        uint32_t* const slot = env->lock_table + i * 2;
        if (slot[0] != 0 && slot[1] != 1) {
            env->delete_critical_section((void*)(uintptr_t)slot[0]);
            env->free_((void*)(uintptr_t)slot[0]);
            slot[0] = 0;
        }
    }
    for (uint32_t i = 0; i < env->entry_count; ++i) {
        uint32_t* const slot = env->lock_table + i * 2;
        if (slot[0] != 0 && slot[1] == 1)
            env->delete_critical_section((void*)(uintptr_t)slot[0]);
    }
}

// +0x3B1C8 DAT_6ff3b1c8: the module's run-time error-check initialiser table. Retail's walk bounds are
// this same address on both sides, so the shipped image registers no initialisers and the loop makes no
// calls; the slot exists so the walk is written exactly as retail's.
static void (*g_bn_rtc_init_table[1])(void);

// 1.13c BNClient+0x03194 (61 B, caller 0x1FDD __CRT_INIT@12): __RTC_Initialize. Walk the RTC
// initialiser table [0x6FF3B1C8, 0x6FF3B1C8) and call every non-null entry. Empty in this image. The 61
// bytes are mostly the injected __SEH_prolog/__SEH_epilog glue.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_3194(void) {
    for (void (**p)(void) = g_bn_rtc_init_table; p < g_bn_rtc_init_table; ++p)
        if (*p != nullptr) (*p)();
}

// 1.13c BNClient+0x0321C (9 B, caller 0x343C __mtinit): the TlsAlloc wrapper installed as the FlsAlloc
// fallback (DAT_6ff3e4d0) when kernel32 lacks FlsAlloc. Ghidra types it void because retail keeps the
// index in EAX rather than storing it; callers read that EAX back, so it is the return here.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_bnclient_321c(uint32_t (*tls_alloc)(void)) {
    return tls_alloc();
}

struct BnMtTermEnv {
    int32_t* tls_index;                      // +0x3D1B0 DAT_6ff3d1b0, the FlsAlloc/TlsAlloc index
    int32_t (*fls_free)(int32_t index);      // +0x3E4DC DAT_6ff3e4dc (FlsFree/TlsFree)
};

// 1.13c BNClient+0x03225 (29 B, callers 0x1FDD/0x343C): __mtterm. When the thread-local slot was
// allocated (index != -1) free it and set it back to -1, then delete every lock-table critical section.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_3225(const BnMtTermEnv* env, const BnMtLocksEnv* locks) {
    if (*env->tls_index != -1) {
        env->fls_free(*env->tls_index);
        *env->tls_index = -1;
    }
    d2_fn_bnclient_3059(locks);
}
