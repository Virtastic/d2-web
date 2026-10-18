// src/core/realm/mcp_crt_init.cpp -- one UNMAPPED 1.13c D2MCPClient.dll row reimplemented from the
// decompile in build-wasm/ghidra-d2mcpclient/decomp/0x11B8.c (alpha lane). Base 0x6FA20000; every RVA
// and global below is retail's. Prior art: the VC7.1 CRT leaves already recovered in
// src/core/realm/mcp_crt_mem.cpp (0x1C25 calloc) and src/core/realm/bnclient_batch2_crt.cpp; the
// runtime itself is Visual Studio 2003 Release (Ghidra "Library Function - Single Match").
//
// The row is MSVC 2003's `__CRT_INIT@12`, the CRT body a DLL's DllMain runs for it: process
// attach/detach and thread attach/detach, returning TRUE (nonzero) when the reason was handled.
// Everything it drives -- kernel32 (GetVersionExA, GetCommandLineA, GetCurrentThreadId), the CRT's
// heap/io/locale/argv/environment initialisers and terminators, and the per-thread data allocator at
// 0x1C25 -- is host or module state no argument pins, so each is a seam in D2McpCrtInitEnv and the
// globals it writes live there too. The rule below is only which seam runs, in what order, and what
// is returned. The two four-byte stack locals the decompile writes ('\b','\x13',-0x5e,'o' and its
// sibling) are the SEH registration frame, not program data.

#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

// OSVERSIONINFOA: the size field, dwMajorVersion, dwMinorVersion, dwBuildNumber, dwPlatformId, then
// szCSDVersion[128]. GetVersionExA is handed the whole 0x94-byte buffer.
struct D2McpOsVersionInfo {
    uint32_t size;        // +0x00
    uint32_t major;       // +0x04
    uint32_t minor;       // +0x08
    uint32_t build;       // +0x0C
    uint32_t platform;    // +0x10
    uint8_t  csd[0x80];   // +0x14
};
static_assert(sizeof(D2McpOsVersionInfo) == 0x94, "1.13c OSVERSIONINFOA layout");

// _tiddata as 0x1C25 allocates it (0x8C bytes); only the two fields this body sets are named.
struct D2McpTiddata {
    uint32_t tid;        // +0x00
    uint32_t thandle;    // +0x04
};
static_assert(__builtin_offsetof(D2McpTiddata, tid) == 0x0 &&
              __builtin_offsetof(D2McpTiddata, thandle) == 0x4, "1.13c VC7.1 _tiddata layout");

struct D2McpCrtInitEnv {
    // --- host seams
    int32_t  (*get_version_ex)(D2McpOsVersionInfo* osvi);   // GetVersionExA
    int32_t  (*heap_init)(void);                            // __heap_init
    int32_t  (*mtinit)(void);                               // __mtinit
    void     (*rtc_initialize)(void);                       // __RTC_Initialize
    const char* (*get_command_line)(void);                  // GetCommandLineA
    void*    (*get_environment_strings)(void);              // ___crtGetEnvironmentStringsA
    int32_t  (*ioinit)(void);                               // __ioinit
    int32_t  (*setargv)(void);                              // __setargv
    int32_t  (*setenvp)(void);                              // __setenvp
    int32_t  (*cinit)(int32_t);                             // __cinit(0)
    void     (*cexit)(void);                                // __cexit
    void     (*ioterm)(void);                               // __ioterm
    void     (*mtterm)(void);                               // __mtterm
    void     (*heap_term)(void);                            // __heap_term
    void*    (*calloc)(uint32_t count, uint32_t size);      // 0x1C25 calloc
    void     (*free)(void* p);                              // _free
    int32_t  (*initptd)(void* ptd, void* locale);           // __initptd
    uint32_t (*get_current_thread_id)(void);                // GetCurrentThreadId
    int32_t  (*thread_attach_check)(void);                  // (*DAT_6fa2ab98)()
    void     (*freeptd)(void* ptd);                         // __freeptd
    // --- module state the body reads and writes
    uint32_t os_platform;         // +0x9E4  DAT_6fa2a9e4
    uint32_t win_major;           // +0x9F0  DAT_6fa2a9f0
    uint32_t win_minor;           // +0x9F4  DAT_6fa2a9f4
    uint32_t os_build;            // +0x9E8  DAT_6fa2a9e8 (+0x8000 when not VER_PLATFORM_WIN32_NT)
    uint32_t os_version;          // +0x9EC  DAT_6fa2a9ec
    const char* command_line;     // +0x125C0 DAT_6fa325c0
    void* environment_strings;    // +0xA2C  DAT_6fa2aa2c
    int32_t attach_count;         // +0xA28  DAT_6fa2aa28
    uint32_t managed_app;         // +0xA20  DAT_6fa2aa20 (0 => __cexit on detach)
    void* locale;                 // +0x2158 DAT_6fa2a158
};

// 1.13c D2MCPClient+0x011B8 (385 B, caller 0x1339 DllMain): `__CRT_INIT@12`. Reason 1 (attach): query
// the OS version once with dwOSVersionInfoSize = 0x94; on success publish platform, major, minor, the
// build with bit 15 clear (and bit 15 set unless the platform is 2), and major*0x100+minor. Then
// __heap_init, __mtinit, __RTC_Initialize, GetCommandLineA, the environment strings, __ioinit,
// __setargv, __setenvp, __cinit(0), in that order; each failure unwinds the initialisers already run
// (__ioterm only after a successful __ioinit) and returns 0. Success increments the attach count and
// returns 1. Reason 0 (detach): only with a positive attach count, decrement it, run __cexit when the
// managed-app flag is clear, then __ioterm, __mtterm, __heap_term, and return 1; otherwise 0.
// Reason 2 (thread attach): 0x1C25(1, 0x8C) for the thread data; when that succeeds and the
// per-thread-init hook reports success, __initptd(data, locale), thandle = -1, tid = the current
// thread id, return 1; a hook failure frees the data and returns 0. Reason 3 (thread detach):
// __freeptd(NULL), return 1. Any other reason returns 1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_crt_init(void* dll_handle, uint32_t reason, void* reserved,
                                             D2McpCrtInitEnv* env) {
    (void)dll_handle;
    (void)reserved;
    if (reason == 1) {                                   // DLL_PROCESS_ATTACH
        D2McpOsVersionInfo osvi{};
        osvi.size = 0x94;
        if (env->get_version_ex(&osvi) == 0) return 0;
        env->os_platform = osvi.platform;
        env->win_major = osvi.major;
        env->win_minor = osvi.minor;
        env->os_build = osvi.build & 0x7fffu;
        if (osvi.platform != 2) env->os_build |= 0x8000u;
        env->os_version = osvi.major * 0x100u + osvi.minor;
        if (env->heap_init() == 0) return 0;
        if (env->mtinit() == 0) {
            env->heap_term();
            return 0;
        }
        env->rtc_initialize();
        env->command_line = env->get_command_line();
        env->environment_strings = env->get_environment_strings();
        if (env->ioinit() >= 0) {
            if (env->setargv() >= 0 && env->setenvp() >= 0 && env->cinit(0) == 0) {
                env->attach_count += 1;
                return 1;
            }
            env->ioterm();
        }
        env->mtterm();
        env->heap_term();
        return 0;
    }
    if (reason == 0) {                                   // DLL_PROCESS_DETACH
        if (env->attach_count <= 0) return 0;
        env->attach_count -= 1;
        if (env->managed_app == 0) env->cexit();
        env->ioterm();
        env->mtterm();
        env->heap_term();
        return 1;
    }
    if (reason == 2) {                                   // DLL_THREAD_ATTACH
        D2McpTiddata* ptd = (D2McpTiddata*)env->calloc(1, 0x8c);
        if (ptd == nullptr) return 0;
        void* locale = env->locale;
        if (env->thread_attach_check() != 0) {
            env->initptd(ptd, locale);
            const uint32_t tid = env->get_current_thread_id();
            ptd->thandle = 0xffffffffu;
            ptd->tid = tid;
            return 1;
        }
        env->free(ptd);
        return 0;
    }
    if (reason == 3) {                                   // DLL_THREAD_DETACH
        env->freeptd(nullptr);
        return 1;
    }
    return 1;
}

// 1.13c D2MCPClient+0x1654 (row s3646, size 40, caller 0x105A __cinit via the C-initializer table slot
// +0xA010, callee 0x28E8 _malloc): the MSVC 2003 Release CRT `___onexitinit`. Allocate the onexit
// callback table, 0x80 bytes = 32 slots; on allocation failure return 0x18 (24, _RT_ENOMEM). Otherwise
// store the block in the table head +0x125D0 (DAT_6fa325d0), zero its first slot (the list terminator),
// and set the end pointer +0x125CC (DAT_6fa325cc) to the same block; return 0. That head/end pair is the
// list d2_mcp_doexit walks (mcp_exit.cpp); retail registers 0x170A with _atexit just after this runs.
static void* g_mcp_onexit_head;   // +0x125D0 DAT_6fa325d0
static void* g_mcp_onexit_end;    // +0x125CC DAT_6fa325cc

extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_onexitinit(void) {
    void* table = std::malloc(0x80);
    if (table == nullptr) return 0x18;
    g_mcp_onexit_head = table;
    *(uint32_t*)table = 0;
    g_mcp_onexit_end = table;
    return 0;
}

// 1.13c D2MCPClient+0x016C6 (61 B, caller 0x011B8 __CRT_INIT): `__RTC_Initialize`. Walks the
// runtime-check initialiser table from DAT_6fa2924c to DAT_6fa2924c and calls every non-null entry
// in order. In this build the two bounds are the same address, so the table is empty and the body
// calls nothing; the loop is still the rule. The 61 bytes are mostly the injected __SEH_prolog /
// __SEH_epilog glue, not program data. Pure in its two bounds; returns how many hooks ran.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_rtc_initialize(void (**begin)(void), void (**end)(void)) {
    uint32_t called = 0;
    for (void (**slot)(void) = begin; slot < end; ++slot) {
        if (*slot != nullptr) {
            (*slot)();
            ++called;
        }
    }
    return called;
}
