// src/core/d2client/d2client_batch_74de.cpp -- one UNMAPPED 1.13c D2Client.dll row reimplemented from
// build-wasm/ghidra-d2client/decomp/0x74DE.c (alpha lane). Base 0x6FAB0000; every global below is named
// at its retail RVA. Prior art: the same 239-byte VC7.1 `__mtinit` body already recovered for
// D2MCPClient (src/core/realm/mcp_crt_batch_b56.cpp:140, same control flow modulo the globals), and the
// D2Client callees are already in this tree -- __mtinitlocks 0x470D (d2client_batch_03f82.cpp), the
// TlsAlloc thunk 0x72BE (d2client_batch_4857.cpp), calloc 0x8797 (d2client_batch_77a1.cpp) and the
// FLS destructor __freefls@4 0x7368 (d2client_batch_7368.cpp).
//
// The four Fls*/Tls* entry slots (+0x0FAA60..6C) and the key (+0x0DB774) are module state no argument
// pins, so they are a caller-owned struct (the MCP shape); kernel32 and the per-thread block are host
// seams. 
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// 0x6FB8B7B8 base 0x6FAB0000 = +0x0DB7B8 DAT_6fb8b7b8: the CRT default per-thread block. Its address
// is what the first _tiddata stores at +0x54 (owned by d2client_batch_7368.cpp).
extern uint8_t g_default_ptd;

typedef uint32_t (*D2ClientFlsAllocFn)(void (*callback)(void*));
typedef int32_t  (*D2ClientFlsGetFn)(uint32_t index);
typedef int32_t  (*D2ClientFlsSetFn)(uint32_t index, void* value);
typedef uint32_t (*D2ClientFlsFreeFn)(uint32_t index);

// The FLS state __mtinit installs. Retail slots and key, all zero in the shipped image.
struct D2ClientFlsState {
    D2ClientFlsAllocFn alloc; // +0x0FAA60 DAT_6fbaaa60 FlsAlloc (0x72BE TlsAlloc thunk fallback)
    D2ClientFlsGetFn   get;   // +0x0FAA64 DAT_6fbaaa64 FlsGetValue (TlsGetValue fallback)
    D2ClientFlsSetFn   set;   // +0x0FAA68 DAT_6fbaaa68 FlsSetValue (TlsSetValue fallback)
    D2ClientFlsFreeFn  free_; // +0x0FAA6C DAT_6fbaaa6c FlsFree (TlsFree fallback)
    uint32_t           key;   // +0x0DB774 DAT_6fb8b774 the FLS key; 0xFFFFFFFF = none
};

// Callees not recovered as callable wasm bodies plus the kernel32 entry points.
struct D2ClientMtInitHost {
    int32_t  (*mtinitlocks)(void);                            // 0x0470D
    void     (*mtterm)(void);                                 // 0x072C7
    void*    (*calloc_)(uint32_t count, uint32_t size);       // 0x08797 calloc(1, 0x8C)
    void*    (*get_module_handle)(const char* name);          // GetModuleHandleA
    void*    (*get_proc_address)(void* module, const char* name); // GetProcAddress
    D2ClientFlsGetFn  tls_get;                                // TlsGetValue import
    D2ClientFlsSetFn  tls_set;                                // TlsSetValue import
    D2ClientFlsAllocFn tls_alloc;                             // 0x072BE TlsAlloc thunk, FlsAlloc signature
    D2ClientFlsFreeFn tls_free;                               // TlsFree import
    void     (*freefls)(void* ptd);                           // 0x07368 __freefls@4, the FLS destructor
    uint32_t (*get_current_thread_id)(void);                  // GetCurrentThreadId
};

// VC7.1 _tiddata, 0x8C bytes as calloc(1, 0x8C) allocates it; only the fields this body writes.
struct D2ClientTiddata74 {
    uint32_t tid;      // +0x00 _tid
    uint32_t thandle;  // +0x04 _thandle = -1
    uint8_t  gap08[0x14 - 0x08];
    uint32_t holdrand; // +0x14 _holdrand = 1
    uint8_t  gap18[0x54 - 0x18];
    uint32_t sentinel; // +0x54 address of the CRT default block (+0x0DB7B8)
    uint8_t  tail[0x8C - 0x58];
};
static_assert(sizeof(D2ClientTiddata74) == 0x8C, "1.13c VC7.1 _tiddata size");
static_assert(offsetof(D2ClientTiddata74, tid) == 0x00, "ptd +0x00 _tid");
static_assert(offsetof(D2ClientTiddata74, thandle) == 0x04, "ptd +0x04 _thandle");
static_assert(offsetof(D2ClientTiddata74, holdrand) == 0x14, "ptd +0x14 _holdrand");
static_assert(offsetof(D2ClientTiddata74, sentinel) == 0x54, "ptd +0x54");

// 1.13c D2Client+0x074DE (239 B, caller 0x04475 __CRT_INIT): `__mtinit`. Initialise the CRT lock table
// (0x470D; failure -> __mtterm, return 0). With a kernel32 handle, resolve FlsAlloc/FlsGetValue/
// FlsSetValue/FlsFree into the four slots; when FlsGetValue is absent, replace all four with the Tls*
// fallbacks (the 0x72BE thunk in the alloc slot). Allocate the FLS key with the resolved allocator and
// the 0x7368 destructor; on failure -> __mtterm, return 0. Then calloc one 0x8C-byte _tiddata, bind it
// to the key and, when the bind succeeds, seed it: +0x54 = the CRT default block's address, +0x14
// (_holdrand) = 1, +0x04 (_thandle) = -1, +0x00 (_tid) = the current thread id. Return 1; otherwise
// __mtterm and return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_74de(D2ClientFlsState* k, const D2ClientMtInitHost* host) {
    if (host->mtinitlocks() == 0) {
        host->mtterm();
        return 0;
    }
    // Retail jumps through a NULL alloc slot when kernel32 is absent; unreachable in the shipped image.
    void* kernel32 = host->get_module_handle("kernel32.dll");
    if (kernel32 != nullptr) {
        k->alloc = (D2ClientFlsAllocFn)host->get_proc_address(kernel32, "FlsAlloc");
        k->get   = (D2ClientFlsGetFn)host->get_proc_address(kernel32, "FlsGetValue");
        k->set   = (D2ClientFlsSetFn)host->get_proc_address(kernel32, "FlsSetValue");
        k->free_ = (D2ClientFlsFreeFn)host->get_proc_address(kernel32, "FlsFree");
        if (k->get == nullptr) {
            k->get   = host->tls_get;
            k->set   = host->tls_set;
            k->alloc = host->tls_alloc;
            k->free_ = host->tls_free;
        }
    }
    k->key = k->alloc(host->freefls);
    if (k->key != 0xFFFFFFFFu) {
        D2ClientTiddata74* ptd = (D2ClientTiddata74*)host->calloc_(1, 0x8C);
        if (ptd != nullptr && k->set(k->key, ptd) != 0) {
            ptd->sentinel = (uint32_t)(uintptr_t)&g_default_ptd;
            ptd->holdrand = 1u;
            uint32_t tid = host->get_current_thread_id();
            ptd->thandle = 0xFFFFFFFFu;
            ptd->tid = tid;
            return 1;
        }
    }
    host->mtterm();
    return 0;
}
