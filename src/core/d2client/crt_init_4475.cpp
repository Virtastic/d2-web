// src/core/d2client/crt_init_4475.cpp -- one UNMAPPED 1.13c D2Client.dll row reimplemented from the
// decompile in build-wasm/ghidra-d2client/decomp/0x4475.c (alpha lane). Base 0x6FAB0000; every RVA and
// global below is retail's. Prior art: the same MSVC 2003 `__CRT_INIT@12` body already recovered for
// D2MCPClient in src/core/realm/mcp_crt_init.cpp (385 B, same layout), and the D2Client TLS/ptd leaves
// in src/core/d2client/d2client_batch_908f.cpp (0x72F7, 0x74DE, 0x74AF).
//
// Everything this body drives -- kernel32 (GetVersionExA, GetCommandLineA, GetCurrentThreadId), the
// CRT heap/io/locale/argv/environment initialisers and terminators, 0x8797 calloc, and the per-thread
// data slot (TlsSetValue through the CRT pointer at 0x0FAA68) -- is host or module state no argument
// pins, so each is a seam in D2ClientCrtInitEnv and the globals it writes live there too. The two
// four-byte stack locals the decompile writes (0x6FAB45C5 / 0x6FAB45D9, both inside this function) are
// the SEH registration frame, not program data.
#include <cstdint>
#include <emscripten.h>

// OSVERSIONINFOA: the size field, dwMajorVersion, dwMinorVersion, dwBuildNumber, dwPlatformId, then
// szCSDVersion[128]. GetVersionExA is handed the whole 0x94-byte buffer.
struct D2ClientOsVersionInfo {
    uint32_t size;        // +0x00
    uint32_t major;       // +0x04
    uint32_t minor;       // +0x08
    uint32_t build;       // +0x0C
    uint32_t platform;    // +0x10
    uint8_t  csd[0x80];   // +0x14
};
static_assert(sizeof(D2ClientOsVersionInfo) == 0x94, "1.13c OSVERSIONINFOA layout");

// _tiddata as 0x8797 calloc allocates it (0x8C bytes); only the two fields this body sets are named.
struct D2ClientTiddata {
    uint32_t tid;        // +0x00
    uint32_t thandle;    // +0x04
};
static_assert(__builtin_offsetof(D2ClientTiddata, tid) == 0x0 &&
              __builtin_offsetof(D2ClientTiddata, thandle) == 0x4, "1.13c VC7.1 _tiddata layout");

struct D2ClientCrtInitEnv {
    // --- host seams (retail RVA in the comment where it is a callee)
    int32_t  (*get_version_ex)(D2ClientOsVersionInfo* osvi);   // GetVersionExA
    int32_t  (*heap_init)(void);                               // 0x8C7F __heap_init
    int32_t  (*mtinit)(void);                                  // 0x74DE __mtinit
    void     (*rtc_initialize)(void);                          // 0x544B __RTC_Initialize
    const char* (*get_command_line)(void);                     // GetCommandLineA
    void*    (*get_environment_strings)(void);                 // 0x8B43 ___crtGetEnvironmentStringsA
    int32_t  (*ioinit)(void);                                  // 0x842E __ioinit
    int32_t  (*setargv)(void);                                 // 0x8AA1 __setargv
    int32_t  (*setenvp)(void);                                 // 0x886E __setenvp
    int32_t  (*cinit)(int32_t);                                // 0x3537 __cinit(0)
    void     (*cexit)(void);                                   // 0x3686 __cexit
    void     (*ioterm)(void);                                  // 0x862C __ioterm
    void     (*mtterm)(void);                                  // 0x72C7 __mtterm
    void     (*heap_term)(void);                               // 0x8CD0 __heap_term
    void*    (*calloc)(uint32_t count, uint32_t size);         // 0x8797 calloc
    void     (*free)(void* p);                                 // 0x4891 _free
    int32_t  (*initptd)(void* ptd, void* locale);              // 0x72E4 __initptd
    uint32_t (*get_current_thread_id)(void);                   // GetCurrentThreadId
    int32_t  (*tls_set_value)(uint32_t index, void* value);    // (*DAT_6fbaaa68) 0x0FAA68, TlsSetValue
    void     (*freeptd)(void* ptd);                            // 0x74AF __freeptd
    // --- module state the body reads and writes
    uint32_t os_platform;         // +0x0FA8A4  DAT_6fbaa8a4
    uint32_t win_major;           // +0x0FA8B0  DAT_6fbaa8b0
    uint32_t win_minor;           // +0x0FA8B4  DAT_6fbaa8b4
    uint32_t os_build;            // +0x0FA8A8  DAT_6fbaa8a8 (+0x8000 when not VER_PLATFORM_WIN32_NT)
    uint32_t os_version;          // +0x0FA8AC  DAT_6fbaa8ac
    const char* command_line;     // +0x124AC4  DAT_6fbd4ac4
    void* environment_strings;    // +0x0FA8F0  DAT_6fbaa8f0
    int32_t attach_count;         // +0x0FA8EC  DAT_6fbaa8ec
    uint32_t managed_app;         // +0x0FA8E0  DAT_6fbaa8e0 (0 => __cexit on detach)
    uint32_t tls_index;           // +0x0DB774  DAT_6fb8b774 (the _tiddata TLS slot; -1 = unset)
};

// 1.13c D2Client+0x04475. Reason 1 (attach): query the OS version once with dwOSVersionInfoSize = 0x94;
// on success publish platform, major, minor, the build with bit 15 clear (and bit 15 set unless the
// platform is 2), and major*0x100+minor. Then __heap_init, __mtinit, __RTC_Initialize,
// GetCommandLineA, the environment strings, __ioinit, __setargv, __setenvp, __cinit(0), in that order;
// each failure unwinds the initialisers already run (__ioterm only after a successful __ioinit) and
// returns 0. Success increments the attach count and returns 1. Reason 0 (detach): only with a positive
// attach count, decrement it, run __cexit when the managed-app flag is clear, then __ioterm, __mtterm,
// __heap_term, and return 1; otherwise 0. Reason 2 (thread attach): calloc(1, 0x8C) for the thread
// data; when TlsSetValue(slot, data) reports success, __initptd(data, the value read at 0x0DB774),
// thandle = -1, tid = the current thread id, return 1; a failure frees the data and returns 0.
// Reason 3 (thread detach): __freeptd(NULL), return 1. Any other reason returns 1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_4475(void* dll_handle, uint32_t reason, void* reserved,
                                            D2ClientCrtInitEnv* env) {
    (void)dll_handle;
    (void)reserved;
    if (reason == 1) {                                   // DLL_PROCESS_ATTACH
        D2ClientOsVersionInfo osvi{};
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
        D2ClientTiddata* ptd = (D2ClientTiddata*)env->calloc(1, 0x8c);
        if (ptd == nullptr) return 0;
        const uint32_t locale = env->tls_index;           // decompile reads 0x0DB774 before the call
        if (env->tls_set_value(env->tls_index, ptd) != 0) {
            env->initptd(ptd, (void*)(uintptr_t)locale);
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
