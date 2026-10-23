// src/core/d2game/d2game_crt_free_3c04.cpp -- two 1.13c D2Game.dll MSVC 2003 (VC7.1) Release CRT rows
// from the cached decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order: __freefls@4
// (0x3C04, the 301-byte per-thread data destructor) and __freeptd (0x3D4B, its only caller, which the
// thread-detach path reaches). Alpha lane (OBJECTIVE v8.3): implementation + marker only, no spec, no
// vector. Retail 0x3C04 was an UNMAPPED callee of the 0x3D4B row, so it is taken here.
//
// Prior art: src/core/d2client/d2client_batch_7368.cpp (__freefls) and d2client_batch_74af.cpp
// (__freeptd), plus this module's own CRT batch: 0x5A8D _free (d2game_batch_5a8d.cpp), 0x398E
// ___freetlocinfo (d2game_batch_crt_398e.cpp), __lock 0x402B (d2game_batch_2a6d.cpp) and the two
// unlock thunks 0x3D36/0x3D42 (d2game_crt_b23.cpp).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" void d2game_free_5a8d(void* mem);   // 0x5A8D _free, ours
extern "C" void* d2game_crt_locale_from_token(uint32_t token); // modeled records from 0x3A5E

static inline uint32_t rd32(const void* p, uint32_t o) {
    uint32_t v;
    std::memcpy(&v, (const uint8_t*)p + o, 4);
    return v;
}
static inline void wr32(void* p, uint32_t o, uint32_t v) {
    std::memcpy((uint8_t*)p + o, &v, 4);
}

// Callees of 0x3C04 in retail call order (the SEH prologue/epilogue 0x40E4/0x411F have no wasm
// analogue). The CRT default locale/table addresses are module state, so they arrive from the caller.
struct D2GameFreeflsEnv {
    void     (*lock_)(int32_t);        // 0x402B __lock
    void     (*unlockD)(void);         // 0x3D36 __unlock(0xD)
    void     (*unlockC)(void);         // 0x3D42 __unlock(0xC)
    void     (*freetlocinfo)(uint32_t); // 0x398E ___freetlocinfo(locale)
    uint32_t default_mbc;              // 0x6FD39FD4 the CRT default mbc-info block
};

// 1.13c D2Game+0x03C04
// __freefls@4(ptd): free the six owned strings at +0x24/+0x2C/+0x34/+0x3C/+0x44/+0x48 and the +0x54
// init address when it is not the CRT default (stored as RVA 0xFF760, retail 0x6FD1F760); under lock
// 0xD drop the mbc-info refcount at +0x60 and free the block at zero when it is not the default; under
// lock 0xC drop the thread locale's refcount and the refcounts at its +0x2C/+0x34/+0x30/+0x40 and the
// +0xB4 count of its +0x4C child, deep-freeing at zero when it is not a default; then free the block.
// A null block is a no-op.
//
// Host model: the thread block's +0x64 holds a locale token, not a pointer (1 = process default,
// 2 = static record at 0x6FD1F078; see d2game_batch_crt_b56.cpp). Resolve those tokens to the same
// modeled records that 0x3A5E updates. Retail decrements the locale and child counts for either default
// too; only the final free is suppressed for those two records. Any other nonzero value is a real
// locale pointer and takes the same walk.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3c04(uint8_t* ptd, const D2GameFreeflsEnv* env) {
    if (ptd == nullptr) return;
    static const uint32_t kOwned[] = { 0x24, 0x2C, 0x34, 0x3C, 0x44, 0x48 };
    for (uint32_t off : kOwned) {
        const uint32_t p = rd32(ptd, off);
        if (p != 0) d2game_free_5a8d((void*)(uintptr_t)p);
    }
    const uint32_t initaddr = rd32(ptd, 0x54);
    if (initaddr != 0xFF760u) d2game_free_5a8d((void*)(uintptr_t)initaddr);

    env->lock_(0xD);
    const uint32_t mbc = rd32(ptd, 0x60);
    if (mbc != 0) {
        wr32((void*)(uintptr_t)mbc, 0, rd32((void*)(uintptr_t)mbc, 0) - 1);
        if (rd32((void*)(uintptr_t)mbc, 0) == 0 && mbc != env->default_mbc)
            d2game_free_5a8d((void*)(uintptr_t)mbc);
    }
    env->unlockD();

    env->lock_(0xC);
    const uint32_t locToken = rd32(ptd, 0x64);
    if (locToken != 0) {
        const bool isDefault = locToken == 1 || locToken == 2;
        uint8_t* loc = isDefault
            ? (uint8_t*)d2game_crt_locale_from_token(locToken)
            : (uint8_t*)(uintptr_t)locToken;
        if (loc != nullptr) {
            wr32(loc, 0, rd32(loc, 0) - 1);
            static const uint32_t kTables[] = { 0x2C, 0x34, 0x30, 0x40 };
            for (uint32_t off : kTables) {
                const uint32_t p = rd32(loc, off);
                if (p != 0) wr32((void*)(uintptr_t)p, 0, rd32((void*)(uintptr_t)p, 0) - 1);
            }
            const uint32_t lc = rd32(loc, 0x4C);
            if (lc != 0) wr32((void*)(uintptr_t)lc, 0xB4, rd32((void*)(uintptr_t)lc, 0xB4) - 1);
            if (rd32(loc, 0) == 0 && !isDefault) env->freetlocinfo(locToken);
        }
    }
    env->unlockC();

    d2game_free_5a8d(ptd);
}

// Callers of 0x3D4B supply the TLS slot and the 0x3C04 environment.
struct D2GameFreeptdEnv {
    uint32_t tls_index;                         // 0x6FD1F204, 0xFFFFFFFF = TLS_OUT_OF_INDEXES
    void*    (*tls_get_value)(uint32_t index);  // TlsGetValue
    int32_t  (*tls_set_value)(uint32_t index, void* value); // TlsSetValue
    D2GameFreeflsEnv freefls;                   // 0x3C04's callees
};

// 1.13c D2Game+0x03D4B
// __freeptd(ptd): with an allocated TLS slot, fetch the calling thread's _tiddata when none was passed,
// run __freefls@4 on it, then clear the slot with TlsSetValue(index, NULL). The slot is -1 when the CRT
// never initialized, in which case nothing happens.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3d4b(void* ptd, const D2GameFreeptdEnv* env) {
    if (env->tls_index == 0xFFFFFFFFu) return;
    if (ptd == nullptr) ptd = env->tls_get_value(env->tls_index);
    d2_fn_d2game_3c04((uint8_t*)ptd, &env->freefls);
    env->tls_set_value(env->tls_index, nullptr);
}
