// src/core/realm/mcp_freetlocinfo.cpp -- one UNMAPPED 1.13c D2MCPClient.dll row (alpha lane) reimplemented
// from build-wasm/ghidra-d2mcpclient/decomp/0x2E3B.c. Retail base 0x6FA20000; every global below is a
// static named at its retail RVA.
//
// MSVC 2003 (VC7.1) Release CRT `___freetlocinfo(plocinfo)`: release the referenced locale tables whose
// refcount reached zero and which are not the CRT defaults, then the block itself. Prior art: the
// byte-identical bodies in src/core/realm/bnclient_batch_030c3.cpp (___freetlocinfo BNClient+0x3601) and
// src/core/d2client/d2client_batch_b56.cpp (D2Client+0x70F2). The four deep-free callees and `_free` are
// seams; the default-pointer globals are arguments because they live in retail's .data.
#include <cstdint>
#include <emscripten.h>

struct D2McpFreeTlocinfoEnv {
    uint32_t  default_locinfo;                     // +0x2AD38  DAT_6fa2ad38 (collate/ctype default)
    uint32_t  default_lconv_mon;                   // +0x32244  DAT_6fa32244
    uint32_t  default_lconv_num;                   // +0x32248  DAT_6fa32248
    uint32_t  default_lc_time;                     // +0x32240  DAT_6fa32240 (the +0x40 block default)
    uint32_t  lc_time_global;                      // +0x2AD34  DAT_6fa2ad34 (the +0x50 record default)
    void     (*free)(void* p);                     // 0x1BB4 _free
    void     (*free_lconv_mon)(uint32_t locinfo);  // 0x49C2 ___free_lconv_mon
    void     (*free_lconv_num)(uint32_t locinfo);  // 0x4963 ___free_lconv_num
    void     (*free_lc_time)(void* lc_time);       // 0x47D3 ___free_lc_time
};

// 1.13c D2MCPClient+0x02E3B (208 B, callers 0x194F __freefls, 0x2F0B ___updatetlocinfo_lk).
// +0x3C is the collate/codepage table; when it is neither the default nor null and the +0x2C string's
// refcount is zero, free the mon table (+0x34) and the num table (+0x30) when each is non-default and
// unreferenced (running its deep free against +0x3C), then the +0x2C string and +0x3C table.
// +0x40 (with base +0x44-0xFE) and +0x50 (refcount at +0xB4) follow under the same rule. Free the block.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_freetlocinfo(uint32_t plocinfo, const D2McpFreeTlocinfoEnv* env) {
    const uint32_t p3c = *(uint32_t*)(uintptr_t)(plocinfo + 0x3C);
    const uint32_t p2c = *(uint32_t*)(uintptr_t)(plocinfo + 0x2C);
    if (p3c != env->default_locinfo && p3c != 0 && *(uint32_t*)(uintptr_t)p2c == 0) {
        uint32_t p = *(uint32_t*)(uintptr_t)(plocinfo + 0x34);
        if (p != 0 && *(uint32_t*)(uintptr_t)p == 0 && p != env->default_lconv_mon) {
            env->free((void*)(uintptr_t)p);
            env->free_lconv_mon(p3c);
        }
        p = *(uint32_t*)(uintptr_t)(plocinfo + 0x30);
        if (p != 0 && *(uint32_t*)(uintptr_t)p == 0 && p != env->default_lconv_num) {
            env->free((void*)(uintptr_t)p);
            env->free_lconv_num(p3c);
        }
        env->free((void*)(uintptr_t)p2c);
        env->free((void*)(uintptr_t)p3c);
    }
    uint32_t p = *(uint32_t*)(uintptr_t)(plocinfo + 0x40);
    if (p != env->default_lc_time && p != 0 && *(uint32_t*)(uintptr_t)p == 0) {
        env->free((void*)(uintptr_t)p);
        env->free((void*)(uintptr_t)(*(uint32_t*)(uintptr_t)(plocinfo + 0x44) - 0xFE));
    }
    p = *(uint32_t*)(uintptr_t)(plocinfo + 0x50);
    if (p != env->lc_time_global && p != 0 && *(uint32_t*)(uintptr_t)(p + 0x2D * 4) == 0) {
        env->free_lc_time((void*)(uintptr_t)p);
        env->free((void*)(uintptr_t)p);
    }
    env->free((void*)(uintptr_t)plocinfo);
}
