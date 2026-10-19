// Prior art used: D2MOO D2Gfx/src/DllMain.cpp for startup context, and the
// existing D2Client VC7.1 __mtinit implementation for the CRT layout. The
// cached D2Gfx 1.13c decompile determines this function's call order.
#include <cstddef>
#include <cstdint>

using D2GfxFlsAlloc = uint32_t (*)(void (*)(void*));
using D2GfxFlsSet = int32_t (*)(uint32_t, void*);

struct D2GfxMtState {
    D2GfxFlsAlloc alloc;  // retail +0x10F50
    void* get;            // retail +0x10F54
    D2GfxFlsSet set;     // retail +0x10F58
    void* free_;          // retail +0x10F5C
    uint32_t key;         // retail +0x10170
};

struct D2GfxMtHost {
    int32_t (*init_locks)();
    void (*term)();
    void* (*module)(const char*);
    void* (*proc)(void*, const char*);
    D2GfxFlsAlloc tls_alloc;
    void* tls_get;
    D2GfxFlsSet tls_set;
    void* tls_free;
    void (*free_fls)(void*);
    void* (*calloc_)(uint32_t, uint32_t);
    uint32_t (*thread_id)();
    uint32_t default_ptd_address; // retail address of DAT_6fa901a0
};

struct D2GfxMtTiddata {
    uint32_t tid;
    uint32_t handle;
    uint8_t gap08[0x14 - 0x08];
    uint32_t holdrand;
    uint8_t gap18[0x54 - 0x18];
    uint32_t default_ptd;
    uint8_t tail[0x8c - 0x58];
};
static_assert(sizeof(D2GfxMtTiddata) == 0x8c);
static_assert(offsetof(D2GfxMtTiddata, default_ptd) == 0x54);

// 1.13c D2Gfx+0x02AE5 (__mtinit): initialize CRT locks, resolve FLS with TLS
// fallback when FlsGetValue is absent, allocate a thread key and a zeroed
// 0x8c-byte thread record, then bind and seed that record. Any failed step
// terminates the CRT lock state and returns zero.
extern "C" int32_t d2gfx_mtinit(D2GfxMtState* state, const D2GfxMtHost* host) {
    if (host->init_locks() == 0) {
        host->term();
        return 0;
    }
    void* kernel32 = host->module("kernel32.dll");
    if (kernel32 != nullptr) {
        state->alloc = reinterpret_cast<D2GfxFlsAlloc>(host->proc(kernel32, "FlsAlloc"));
        state->get = host->proc(kernel32, "FlsGetValue");
        state->set = reinterpret_cast<D2GfxFlsSet>(host->proc(kernel32, "FlsSetValue"));
        state->free_ = host->proc(kernel32, "FlsFree");
        if (state->get == nullptr) {
            state->alloc = host->tls_alloc;
            state->get = host->tls_get;
            state->set = host->tls_set;
            state->free_ = host->tls_free;
        }
    }
    state->key = state->alloc(host->free_fls);
    if (state->key != 0xffffffffu) {
        auto* ptd = static_cast<D2GfxMtTiddata*>(host->calloc_(1, 0x8c));
        if (ptd != nullptr && state->set(state->key, ptd) != 0) {
            ptd->default_ptd = host->default_ptd_address;
            ptd->holdrand = 1;
            const uint32_t tid = host->thread_id();
            ptd->handle = 0xffffffffu;
            ptd->tid = tid;
            return 1;
        }
    }
    host->term();
    return 0;
}
