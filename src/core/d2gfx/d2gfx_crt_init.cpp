// D2Gfx VC7.1 CRT lifecycle, validated against the cached 1.13c 0x1686 decompile.
// Prior art: D2MOO D2Gfx/DllMain.cpp covers the user entry only; existing MCP CRT
// lifecycle supplies the callback seam pattern. Module/OS state is explicit in env.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

// OSVERSIONINFOA: the size field, dwMajorVersion, dwMinorVersion, dwBuildNumber, dwPlatformId, then
// szCSDVersion[128]. GetVersionExA is handed the whole 0x94-byte buffer.
struct D2GfxOsVersionInfo {
    uint32_t size;        // +0x00
    uint32_t major;       // +0x04
    uint32_t minor;       // +0x08
    uint32_t build;       // +0x0C
    uint32_t platform;    // +0x10
    uint8_t  csd[0x80];   // +0x14
};
static_assert(sizeof(D2GfxOsVersionInfo) == 0x94, "1.13c OSVERSIONINFOA layout");

// _tiddata as 0x2C45 allocates it (0x8C bytes); only the two fields this body sets are named.
struct D2GfxTiddata {
    uint32_t tid;        // +0x00
    uint32_t thandle;    // +0x04
};
static_assert(__builtin_offsetof(D2GfxTiddata, tid) == 0x0 &&
              __builtin_offsetof(D2GfxTiddata, thandle) == 0x4, "1.13c VC7.1 _tiddata layout");

struct D2GfxCrtInitEnv {
    // --- host seams
    int32_t  (*get_version_ex)(D2GfxOsVersionInfo* osvi);   // GetVersionExA
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
    void*    (*calloc)(uint32_t count, uint32_t size);      // 0x2C45 calloc
    void     (*free)(void* p);                              // _free
    int32_t  (*initptd)(void* ptd, void* locale);           // __initptd
    uint32_t (*get_current_thread_id)(void);                // GetCurrentThreadId
    int32_t  (*thread_attach_check)(void);                  // (*DAT_6fa90f58)()
    void     (*freeptd)(void* ptd);                         // __freeptd
    // --- module state the body reads and writes
    uint32_t os_platform;         // +0x10DA4  DAT_6fa90da4
    uint32_t win_major;           // +0x10DB0  DAT_6fa90db0
    uint32_t win_minor;           // +0x10DB4  DAT_6fa90db4
    uint32_t os_build;            // +0x10DA8  DAT_6fa90da8 (+0x8000 when not VER_PLATFORM_WIN32_NT)
    uint32_t os_version;          // +0x10DAC  DAT_6fa90dac
    const char* command_line;     // +0x1EA20 DAT_6fa9ea20
    void* environment_strings;    // +0x10DEC  DAT_6fa90dec
    int32_t attach_count;         // +0x10DE8  DAT_6fa90de8
    uint32_t managed_app;         // +0x10DE0  DAT_6fa90de0 (0 => __cexit on detach)
    void* locale;                 // +0x10170 DAT_6fa90170
};

// 1.13c D2Gfx+0x01686 (385 B, caller 0x1807 DllMain): `__CRT_INIT@12`. Reason 1 (attach): query
// the OS version once with dwOSVersionInfoSize = 0x94; on success publish platform, major, minor, the
// build with bit 15 clear (and bit 15 set unless the platform is 2), and major*0x100+minor. Then
// __heap_init, __mtinit, __RTC_Initialize, GetCommandLineA, the environment strings, __ioinit,
// __setargv, __setenvp, __cinit(0), in that order; each failure unwinds the initialisers already run
// (__ioterm only after a successful __ioinit) and returns 0. Success increments the attach count and
// returns 1. Reason 0 (detach): only with a positive attach count, decrement it, run __cexit when the
// managed-app flag is clear, then __ioterm, __mtterm, __heap_term, and return 1; otherwise 0.
// Reason 2 (thread attach): 0x2C45(1, 0x8C) for the thread data; when that succeeds and the
// per-thread-init hook reports success, __initptd(data, locale), thandle = -1, tid = the current
// thread id, return 1; a hook failure frees the data and returns 0. Reason 3 (thread detach):
// __freeptd(NULL), return 1. Any other reason returns 1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2gfx_crt_init(void* dll_handle, uint32_t reason, void* reserved,
                                             D2GfxCrtInitEnv* env) {
    (void)dll_handle;
    (void)reserved;
    if (reason == 1) {                                   // DLL_PROCESS_ATTACH
        D2GfxOsVersionInfo osvi{};
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
                env->attach_count = static_cast<int32_t>(static_cast<uint32_t>(env->attach_count) + 1u);
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
        D2GfxTiddata* ptd = (D2GfxTiddata*)env->calloc(1, 0x8c);
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


// 1.13c D2Gfx+0x0323B: the MSVC CRT helper that supplies the environment block consumed by
// __CRT_INIT@12. Prior art: the same VC7.1 CRT body in D2Client+0x8B43 and D2DDraw+0x24FB supplies
// the ported branch and buffer semantics; D2MOO D2Gfx/DllMain.cpp covers the surrounding user entry,
// not this linked CRT helper. The module's probe word is DAT_6fa91068. Keep Win32 calls as explicit
// seams; WCHAR is uint16_t because retail's environment block is UTF-16 regardless of wasm wchar_t.
struct D2GfxEnvStringsEnv {
    void*       (*get_environment_strings_w)(void);
    const char* (*get_environment_strings_a)(void);
    int32_t     (*wide_char_to_multi_byte)(uint32_t cp, const void* wide, int32_t wide_len,
                                           char* out, int32_t out_size);
    uint32_t    (*get_last_error)(void);
    void        (*free_environment_strings_w)(const void* block);
    void        (*free_environment_strings_a)(const char* block);
};
static uint32_t g_d2gfx_env_strings_kind; // +0x10068 DAT_6fa91068: 0 untried, 1 wide, 2 ANSI

extern "C"
EMSCRIPTEN_KEEPALIVE void* d2gfx_crt_get_environment_strings_a(D2GfxEnvStringsEnv* env) {
    const uint16_t* wide = nullptr;
    if (g_d2gfx_env_strings_kind == 0) {
        wide = (const uint16_t*)env->get_environment_strings_w();
        if (wide != nullptr) {
            g_d2gfx_env_strings_kind = 1;
        } else if (env->get_last_error() == 0x78) {
            g_d2gfx_env_strings_kind = 2;
        }
    }
    if (g_d2gfx_env_strings_kind != 1) {
        if (g_d2gfx_env_strings_kind != 2 && g_d2gfx_env_strings_kind != 0) return nullptr;
        const char* ansi = env->get_environment_strings_a();
        if (ansi == nullptr) return nullptr;
        const char* p = ansi;
        while (*p != 0) { while (*p != 0) ++p; ++p; }
        const size_t bytes = (size_t)(p - ansi) + 1;
        void* dst = std::malloc(bytes);
        if (dst != nullptr) std::memcpy(dst, ansi, bytes);
        env->free_environment_strings_a(ansi);
        return dst;
    }
    if (wide == nullptr) {
        wide = (const uint16_t*)env->get_environment_strings_w();
        if (wide == nullptr) return nullptr;
    }
    const uint16_t* p = wide;
    while (*p != 0) { while (*p != 0) ++p; ++p; }
    const int32_t chars = (int32_t)(p - wide) + 1;
    const int32_t bytes = env->wide_char_to_multi_byte(0, wide, chars, nullptr, 0);
    char* dst = bytes != 0 ? (char*)std::malloc((size_t)bytes) : nullptr;
    if (dst != nullptr && env->wide_char_to_multi_byte(0, wide, chars, dst, bytes) == 0) {
        std::free(dst);
        dst = nullptr;
    }
    env->free_environment_strings_w(wide);
    return dst;
}
