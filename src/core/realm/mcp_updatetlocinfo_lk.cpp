// src/core/realm/mcp_updatetlocinfo_lk.cpp -- one UNMAPPED 1.13c D2MCPClient.dll row reimplemented
// from the decompile in build-wasm/ghidra-d2mcpclient/decomp/0x2F0B.c (alpha lane). Base 0x6FA20000;
// every global below is named at its retail RVA. The row is MSVC 2003 (VC7.1) Release
// `___updatetlocinfo_lk`, pulled into the module as a library leaf. Prior art: the byte-identical
// BNClient sibling src/core/realm/bnclient_batch_2558.cpp (0x36D1) and the D2DDraw copy
// src/core/gfx/d2ddraw_batch_10da.cpp; the rules are the same, restated against D2MCPClient's own
// globals. __getptd (0x18DE) and ___freetlocinfo (0x2E3B) are host/module state no argument pins, so
// each is a seam in D2McpCrt2F0BEnv.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// +0xA430 DAT_6fa2a430: the static tlocinfo a release path must never free. Its +0x00 refcount starts
// at 1 and its +0x4C lconv pointer is the non-null static table (the retail .data bytes at 0xA47C).
static uint8_t g_mcp_static_tlocinfo[0x90];
static uint8_t g_mcp_static_lconv[0xB8];
// +0xA484 PTR_DAT_6fa2a484: the process default locale pointer, initialized to the static record.
static int* g_mcp_default_locale;
static bool g_mcp_locale_ready;
// The single-threaded _ptiddata: retail hands out one 0x8C block through __getptd (0x18DE).
static uint8_t g_mcp_ptd[0x8C];
static bool g_mcp_ptd_ready;

struct D2McpCrt2F0BEnv {
    void* (*getptd)(void);          // 0x18DE __getptd
    void  (*freetlocinfo)(void*);   // 0x2E3B ___freetlocinfo
};

// 1.13c D2MCPClient+0x018DE (113 B, callers 0x246D, 0x2838, 0x2F0B, 0x5881): __getptd. Retail saves
// GetLastError, reads the TLS slot DAT_6fa2a158, and on a miss calloc(1,0x8C) (0x1C25), stores it back
// and seeds _initaddr +0x54 = &DAT_6fa2a190 (RVA 0xA190), _holdrand +0x14 = 1, _thandle +0x04 = -1 and
// _tid +0x00 = GetCurrentThreadId(); a failed allocation/store runs __amsg_exit(0x10). The saved error
// is restored before returning. Engine single-threaded => one static block, tid 1; the allocation is
// static, so the fatal path does not exist here.
extern "C" EMSCRIPTEN_KEEPALIVE void* d2_mcp_getptd_18de(void) {
    if (!g_mcp_ptd_ready) {
        std::memset(g_mcp_ptd, 0, sizeof g_mcp_ptd);
        *(uint32_t*)(g_mcp_ptd + 0x54) = 0xA190u;       // _initaddr, retail 0x6FA2A190
        *(uint32_t*)(g_mcp_ptd + 0x14) = 1u;            // _holdrand
        *(uint32_t*)(g_mcp_ptd + 0x04) = 0xFFFFFFFFu;   // _thandle
        *(uint32_t*)(g_mcp_ptd + 0x00) = 1u;            // _tid
        g_mcp_ptd_ready = true;
    }
    return g_mcp_ptd;
}
static void mcp_default_freetlocinfo(void*) {}

static const D2McpCrt2F0BEnv g_mcp_crt_2f0b_default = {d2_mcp_getptd_18de, mcp_default_freetlocinfo};

static int* mcp_load_ptr(const void* base, uint32_t off) {
    int* p;
    std::memcpy(&p, (const uint8_t*)base + off, sizeof p);
    return p;
}
static void mcp_store_ptr(void* base, uint32_t off, int* p) {
    std::memcpy((uint8_t*)base + off, &p, sizeof p);
}

// Retail's file image: the static record's refcount is 1 and +0x4C points at a non-null lconv; the
// +0x2C/+0x30/+0x34/+0x40 sub-object slots are null, so their reference walks are skipped.
static void mcp_locale_init(void) {
    if (g_mcp_locale_ready) return;
    *(int*)g_mcp_static_tlocinfo = 1;
    mcp_store_ptr(g_mcp_static_tlocinfo, 0x4C, (int*)g_mcp_static_lconv);
    g_mcp_default_locale = (int*)g_mcp_static_tlocinfo;
    g_mcp_locale_ready = true;
}

// 1.13c D2MCPClient+0x02F0B (193 B, caller 0x02FCC ___updatetlocinfo): ___updatetlocinfo_lk. Take the
// calling thread's locale pointer (ptd +0x64); when it is not the process default, drop the old
// locale's reference count and those of its sub-objects at +0x2C/+0x30/+0x34/+0x40 and of its lconv at
// +0x4C/+0xB4, install the default and add those references back, then free the old locale at zero
// unless it is the static record at +0xA430. Return the installed pointer.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_updatetlocinfo_lk(const D2McpCrt2F0BEnv* env) {
    if (env == nullptr) env = &g_mcp_crt_2f0b_default;
    mcp_locale_init();
    int** slot = (int**)((uint8_t*)env->getptd() + 0x64);   // _tfpecode / ptlocinfo
    int* loc = *slot;
    int* def = g_mcp_default_locale;
    if (loc != def) {
        if (loc != nullptr) {
            int* p = mcp_load_ptr(loc, 0x2C);
            --*loc;
            if (p != nullptr) --*p;
            p = mcp_load_ptr(loc, 0x34);
            if (p != nullptr) --*p;
            p = mcp_load_ptr(loc, 0x30);
            if (p != nullptr) --*p;
            p = mcp_load_ptr(loc, 0x40);
            if (p != nullptr) --*p;
            p = mcp_load_ptr(loc, 0x4C);
            if (p != nullptr) --*(int*)((uint8_t*)p + 0xB4);
        }
        *slot = def;
        ++*def;
        int* p;
        if ((p = mcp_load_ptr(def, 0x2C)) != nullptr) ++*p;
        if ((p = mcp_load_ptr(def, 0x34)) != nullptr) ++*p;
        if ((p = mcp_load_ptr(def, 0x30)) != nullptr) ++*p;
        if ((p = mcp_load_ptr(def, 0x40)) != nullptr) ++*p;
        if ((p = mcp_load_ptr(def, 0x4C)) != nullptr) ++*(int*)((uint8_t*)p + 0xB4);
        if (loc != nullptr && *loc == 0 && loc != (int*)g_mcp_static_tlocinfo)
            env->freetlocinfo(loc);
    }
    return (int32_t)(intptr_t)*slot;
}
