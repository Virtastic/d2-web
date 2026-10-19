// src/core/realm/bnclient_mtinit_343c.cpp -- one 1.13c BNClient.dll alpha-lane row, transcribed from
// build-wasm/ghidra-bnclient/decomp/0x343C.c (image base 0x6FF20000). MSVC 2003 (VC7.1) Release
// `__mtinit`, the CRT's thread-local-data start-up. The four Fls exports it caches live in module globals
// DAT_6ff3e4d0..dc and the allocated slot index in DAT_6ff3d1b0; those globals arrive as seam pointers in
// retail order. GetModuleHandleA/GetProcAddress and the kernel32 exports itself are the lookup seam the
// host supplies (null lookup = kernel32 unavailable, which retail handles by leaving the slots untouched).
#include <cstdint>
#include <emscripten.h>

typedef uint32_t (*BnFlsAllocFn)(void* callback);            // FlsAlloc, or the TlsAlloc wrapper
typedef void*    (*BnFlsGetValueFn)(uint32_t index);         // FlsGetValue / TlsGetValue
typedef int32_t  (*BnFlsSetValueFn)(uint32_t index, void* value); // FlsSetValue / TlsSetValue
typedef int32_t  (*BnFlsFreeFn)(uint32_t index);             // FlsFree / TlsFree

struct BnMtInitEnv {
    int32_t  (*mtinitlocks)(void);          // 0x3010 __mtinitlocks
    void     (*mtterm)(void);               // 0x3225 __mtterm
    void*    (*get_proc_address)(const char* name); // GetProcAddress(GetModuleHandleA("kernel32.dll"), name)
    BnFlsAllocFn*    fls_alloc;             // +0x3E4D0 DAT_6ff3e4d0
    BnFlsGetValueFn* fls_get_value;         // +0x3E4D4 DAT_6ff3e4d4
    BnFlsSetValueFn* fls_set_value;         // +0x3E4D8 DAT_6ff3e4d8
    BnFlsFreeFn*     fls_free;              // +0x3E4DC DAT_6ff3e4dc
    BnFlsAllocFn     tls_alloc_stub;        // 0x321C, the TlsAlloc wrapper installed as the FlsAlloc fallback
    BnFlsGetValueFn  tls_get_value;         // TlsGetValue
    BnFlsSetValueFn  tls_set_value;         // TlsSetValue
    BnFlsFreeFn      tls_free;              // TlsFree
    void*            freefls;               // __freefls, the callback passed to FlsAlloc
    void*  (*calloc_)(uint32_t count, uint32_t size); // 0x3FB6 ___calloc, the 0x8C-byte thread-data block
    uint32_t (*current_thread_id)(void);    // GetCurrentThreadId
    int32_t* fls_index;                     // +0x3D1B0 DAT_6ff3d1b0, in/out
    uint32_t thread_callback_rva;           // &DAT_6ff3d4d8, stored in the block at +0x54
};

// 1.13c BNClient+0x0343C (239 B, caller 0x1FDD __CRT_INIT@12): __mtinit. Bring up the lock table
// (0x3010); on failure run __mtterm and return 0. With kernel32 present, cache GetProcAddress results for
// FlsAlloc/FlsGetValue/FlsSetValue/FlsFree into the four globals, and when FlsGetValue is missing install
// the TlsAlloc wrapper (0x321C) plus TlsGetValue/TlsSetValue/TlsFree instead. Allocate an Fls slot for the
// __freefls callback and store its index in DAT_6ff3d1b0. A live index then callocates the 0x8C-byte
// thread-data block and binds it to the slot: on success stamp +0x54 with &DAT_6ff3d4d8, +0x14 with 1,
// +0x8 with -1 and +0x0 with the current thread id, then return 1. Any failure after the index is taken
// reaches __mtterm (which frees the slot) and returns 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_bnclient_343c(const BnMtInitEnv* env) {
    if (env->mtinitlocks() == 0) {
        env->mtterm();
        return 0;
    }
    if (env->get_proc_address != nullptr) {
        *env->fls_alloc = reinterpret_cast<BnFlsAllocFn>(env->get_proc_address("FlsAlloc"));
        *env->fls_get_value = reinterpret_cast<BnFlsGetValueFn>(env->get_proc_address("FlsGetValue"));
        *env->fls_set_value = reinterpret_cast<BnFlsSetValueFn>(env->get_proc_address("FlsSetValue"));
        *env->fls_free = reinterpret_cast<BnFlsFreeFn>(env->get_proc_address("FlsFree"));
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
