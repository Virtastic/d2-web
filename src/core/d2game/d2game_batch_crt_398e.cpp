// src/core/d2game/d2game_batch_crt_398e.cpp -- the 1.13c D2Game.dll MSVC 2003 (VC7.1) Release CRT
// locale destructor family from the cached decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in
// RVA order: ___freetlocinfo (0x398E) and the three deep-free helpers it calls -- ___free_lc_time
// (0x6FDA), ___free_lconv_num (0x716A) and ___free_lconv_mon (0x71C9). Alpha batch lane (OBJECTIVE
// v8.3): implementation + marker only, no spec vector.
//
// Prior art: the byte-identical bodies in src/core/realm/mcp_freetlocinfo.cpp (D2MCPClient+0x2E3B),
// src/core/realm/bnclient_batch_030c3.cpp (BNClient+0x3601) and src/core/realm/bnclient_batch_6265.cpp
// (the same three deep frees). Host model follows them: the CRT default-table addresses
// (DAT_6FD3000C etc.) live in retail .data, so they arrive as the env's fields; the three helpers take
// the default lconv table and the static-string table they compare against as arguments. _free
// (0x5A8D) is ours (src/core/d2game/d2game_batch_5a8d.cpp).
#include <cstdint>
#include <emscripten.h>

extern "C" void d2game_free_5a8d(void* mem);   // 0x5A8D _free

// Host seams for the retail CRT defaults the decompiles compare against: 0x6FD3000C (default
// collate/codepage block), 0x6FD3A204/0x6FD3A208 (default lconv mon/num), 0x6FD3A200 (default
// lc_time), 0x6FD30008 (the lc_time global record); 0x6FD1F97C is the default lconv table and
// 0x6FD1F94C/0x6FD1F958 the static string tables.
struct D2GameFreeTlocinfoEnv {
    uint32_t default_locinfo;
    uint32_t default_lconv_mon;
    uint32_t default_lconv_num;
    uint32_t default_lc_time;
    uint32_t lc_time_global;
    uint32_t default_lconv;        // the table at *PTR_PTR_6FD1F97C
    uint32_t static_lconv_num;     // PTR_DAT_6FD1F94C (three strings)
    uint32_t static_lconv_mon;     // PTR_DAT_6FD1F958 (seven strings)
};

// 1.13c D2Game+0x06FDA
// ___free_lc_time(lc_time): free the record's 43 owned string slots in retail order -- 1..6, then 0,
// then 8..0x2A (slot 7 is never touched). A null record frees nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_6FDA(uint32_t* lcTime) {
    if (lcTime == nullptr) return;
    for (int i = 1; i <= 6; ++i) d2game_free_5a8d((void*)(uintptr_t)lcTime[i]);
    d2game_free_5a8d((void*)(uintptr_t)lcTime[0]);
    for (int i = 8; i <= 0x2A; ++i) d2game_free_5a8d((void*)(uintptr_t)lcTime[i]);
}

// 1.13c D2Game+0x0716A
// ___free_lconv_num(loconv): each of the three numeric-format fields (+0x0/+0x4/+0x8) is freed unless
// it is the default lconv entry or the matching static string.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_716A(uint32_t* loconv, const uint32_t* defaultLconv,
                                            const uint32_t* staticNum) {
    if (loconv == nullptr) return;
    for (int i = 0; i < 3; ++i) {
        const uint32_t p = loconv[i];
        if (p != defaultLconv[i] && p != staticNum[i]) d2game_free_5a8d((void*)(uintptr_t)p);
    }
}

// 1.13c D2Game+0x071C9
// ___free_lconv_mon(loconv): each of the seven monetary-format fields (+0xC..+0x24) is freed unless it
// is the default lconv entry or the matching static string.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_71C9(uint32_t* loconv, const uint32_t* defaultLconv,
                                            const uint32_t* staticMon) {
    if (loconv == nullptr) return;
    for (int i = 3; i <= 9; ++i) {
        const uint32_t p = loconv[i];
        if (p != defaultLconv[i] && p != staticMon[i - 3]) d2game_free_5a8d((void*)(uintptr_t)p);
    }
}

// 1.13c D2Game+0x0398E
// ___freetlocinfo(plocinfo): when +0x3C is neither the default nor null and the +0x2C string's
// refcount is zero, free the unreferenced, non-default mon table (+0x34) and num table (+0x30) with
// their deep frees against +0x3C, then the +0x2C string and the +0x3C block. Then +0x40 (with
// +0x44-0xFE) and +0x50 (refcount at +0xB4) follow the same rule. Finally free the block itself.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_398E(uint32_t plocinfo, const D2GameFreeTlocinfoEnv* env) {
    uint32_t p;
    const uint32_t locinfo = *(uint32_t*)(uintptr_t)(plocinfo + 0x3C);
    if (locinfo != env->default_locinfo && locinfo != 0 &&
        *(uint32_t*)(uintptr_t)(*(uint32_t*)(uintptr_t)(plocinfo + 0x2C)) == 0) {
        p = *(uint32_t*)(uintptr_t)(plocinfo + 0x34);
        if (p != 0 && *(uint32_t*)(uintptr_t)p == 0 && p != env->default_lconv_mon) {
            d2game_free_5a8d((void*)(uintptr_t)p);
            d2_fn_d2game_71C9((uint32_t*)(uintptr_t)locinfo,
                              (const uint32_t*)(uintptr_t)env->default_lconv,
                              (const uint32_t*)(uintptr_t)env->static_lconv_mon);
        }
        p = *(uint32_t*)(uintptr_t)(plocinfo + 0x30);
        if (p != 0 && *(uint32_t*)(uintptr_t)p == 0 && p != env->default_lconv_num) {
            d2game_free_5a8d((void*)(uintptr_t)p);
            d2_fn_d2game_716A((uint32_t*)(uintptr_t)locinfo,
                              (const uint32_t*)(uintptr_t)env->default_lconv,
                              (const uint32_t*)(uintptr_t)env->static_lconv_num);
        }
        d2game_free_5a8d((void*)(uintptr_t)(*(uint32_t*)(uintptr_t)(plocinfo + 0x2C)));
        d2game_free_5a8d((void*)(uintptr_t)locinfo);
    }
    p = *(uint32_t*)(uintptr_t)(plocinfo + 0x40);
    if (p != env->default_lc_time && p != 0 && *(uint32_t*)(uintptr_t)p == 0) {
        d2game_free_5a8d((void*)(uintptr_t)p);
        d2game_free_5a8d((void*)(uintptr_t)(*(uint32_t*)(uintptr_t)(plocinfo + 0x44) - 0xFE));
    }
    p = *(uint32_t*)(uintptr_t)(plocinfo + 0x50);
    if (p != env->lc_time_global && p != 0 && *(uint32_t*)(uintptr_t)(p + 0xB4) == 0) {
        d2_fn_d2game_6FDA((uint32_t*)(uintptr_t)p);
        d2game_free_5a8d((void*)(uintptr_t)p);
    }
    d2game_free_5a8d((void*)(uintptr_t)plocinfo);
}
