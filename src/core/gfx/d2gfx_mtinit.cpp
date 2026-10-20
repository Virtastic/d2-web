// src/core/gfx/d2gfx_mtinit.cpp -- 1.13c D2Gfx+0x2AE5 __mtinit, transcribed from
// build-wasm/ghidra-d2gfx/decomp/0x2AE5.c (image base 0x6FA80000). MSVC 2003 (VC7.1) Release
// `__mtinit`, the CRT's thread-local-data start-up. The four Fls exports it caches live in module globals
// DAT_6fa90f50..5c and the allocated slot index in DAT_6fa90170; those globals arrive as seam pointers in
// retail order. GetModuleHandleA/GetProcAddress and the kernel32 exports itself are the lookup seam the
// host supplies (null lookup = kernel32 unavailable, which retail handles by leaving the slots untouched).
#include <cstdint>


typedef uint32_t (*D2gFlsAllocFn)(void* callback);            // FlsAlloc, or the TlsAlloc wrapper
typedef void*    (*D2gFlsGetValueFn)(uint32_t index);         // FlsGetValue / TlsGetValue
typedef int32_t  (*D2gFlsSetValueFn)(uint32_t index, void* value); // FlsSetValue / TlsSetValue
typedef int32_t  (*D2gFlsFreeFn)(uint32_t index);             // FlsFree / TlsFree

struct D2GfxMtInitEnv {
    int32_t  (*mtinitlocks)(void);          // 0x191E __mtinitlocks
    void     (*mtterm)(void);               // 0x28CE __mtterm
    void*    (*get_proc_address)(const char* name); // GetProcAddress(GetModuleHandleA("kernel32.dll"), name)
    D2gFlsAllocFn*    fls_alloc;             // +0x10F50 DAT_6fa90f50
    D2gFlsGetValueFn* fls_get_value;         // +0x10F54 DAT_6fa90f54
    D2gFlsSetValueFn* fls_set_value;         // +0x10F58 DAT_6fa90f58
    D2gFlsFreeFn*     fls_free;              // +0x10F5C DAT_6fa90f5c
    D2gFlsAllocFn     tls_alloc_stub;        // 0x28C5, the TlsAlloc wrapper installed as the FlsAlloc fallback
    D2gFlsGetValueFn  tls_get_value;         // TlsGetValue
    D2gFlsSetValueFn  tls_set_value;         // TlsSetValue
    D2gFlsFreeFn      tls_free;              // TlsFree
    void*            freefls;               // __freefls, the callback passed to FlsAlloc
    void*  (*calloc_)(uint32_t count, uint32_t size); // 0x2C45 calloc, the 0x8C-byte thread-data block
    uint32_t (*current_thread_id)(void);    // GetCurrentThreadId
    int32_t* fls_index;                     // +0x10170 DAT_6fa90170, in/out
    uint32_t thread_callback_rva;           // &DAT_6fa901a0, stored in the block at +0x54
};

// 1.13c D2Gfx+0x2AE5 (239 B, caller 0x1686): __mtinit. Bring up the lock table
// (0x191E); on failure run __mtterm and return 0. With kernel32 present, cache GetProcAddress results for
// FlsAlloc/FlsGetValue/FlsSetValue/FlsFree into the four globals, and when FlsGetValue is missing install
// the TlsAlloc wrapper (0x28C5) plus TlsGetValue/TlsSetValue/TlsFree instead. Allocate an Fls slot for the
// __freefls callback and store its index in DAT_6fa90170. A live index then callocates the 0x8C-byte
// thread-data block and binds it to the slot: on success stamp +0x54 with &DAT_6fa901a0, +0x14 with 1,
// +0x8 with -1 and +0x0 with the current thread id, then return 1. Any failure after the index is taken
// reaches __mtterm (which frees the slot) and returns 0.
extern "C"
int32_t d2gfx_mtinit_env(const D2GfxMtInitEnv* env) {
    if (env->mtinitlocks() == 0) {
        env->mtterm();
        return 0;
    }
    if (env->get_proc_address != nullptr) {
        *env->fls_alloc = reinterpret_cast<D2gFlsAllocFn>(env->get_proc_address("FlsAlloc"));
        *env->fls_get_value = reinterpret_cast<D2gFlsGetValueFn>(env->get_proc_address("FlsGetValue"));
        *env->fls_set_value = reinterpret_cast<D2gFlsSetValueFn>(env->get_proc_address("FlsSetValue"));
        *env->fls_free = reinterpret_cast<D2gFlsFreeFn>(env->get_proc_address("FlsFree"));
        if (*env->fls_get_value == nullptr) {
            *env->fls_get_value = env->tls_get_value;
            *env->fls_set_value = env->tls_set_value;
            *env->fls_alloc = env->tls_alloc_stub;
            *env->fls_free = env->tls_free;
        }
    }
    *env->fls_index = static_cast<int32_t>((*env->fls_alloc)(env->freefls));
    if (*env->fls_index != -1) {
        uint32_t* const ptd = static_cast<uint32_t*>(env->calloc_(1, 0x8C));
        if (ptd != nullptr &&
            (*env->fls_set_value)(static_cast<uint32_t>(*env->fls_index), ptd) != 0) {
            ptd[0x15] = env->thread_callback_rva;
            ptd[5] = 1;
            const uint32_t tid = env->current_thread_id();
            ptd[1] = 0xFFFFFFFFu;
            ptd[0] = tid;
            return 1;
        }
    }
    env->mtterm();
    return 0;
}
