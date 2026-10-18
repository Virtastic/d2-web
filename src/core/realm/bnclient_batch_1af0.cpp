// BNClient.dll alpha-lane batch 3 (rows 0x1AF0..0x215E): MSVC 2003 Release CRT leaves that the module
// statically links. Every rule below is transcribed from the cached Ghidra decompile
// (build-wasm/ghidra-bnclient/decomp) of the retail image (base 0x6FF20000). Each d2_fn_* probe takes its
// arguments in r[0..] and writes its return value in r[4] (0x1F1F, six arguments, uses r[6]).
#include <emscripten.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

// Already implemented by the earlier BNClient CRT batch (src/core/realm/bnclient_batch_rowex.cpp).
extern "C" int bn_tolower_mt(const void*, int);   // 0x1C76 ___tolower_mt
extern "C" void* bn_getptd(void);                 // 0x3255 __getptd
extern "C" void* bn_updatetlocinfo(void);         // 0x3792 ___updatetlocinfo

// --- shared helpers (no retail RVA of their own) ---------------------------------------------------

// type_info's module vtable (retail `&vftable`, module data). The destructor stores it before freeing the
// name pointer; the table itself is data, not a function of this batch.
static const void* g_bn_type_info_vftable[1];

// ~type_info body: store the vtable, __lock(0xE), free the name at +4 when set, then __unlock(0xE). The
// engine is single-threaded, so the CRT lock table (and its 0xE slot) has no analogue here.
static void bn_type_info_destroy(uint8_t* self) {
    if (self == nullptr) return;
    *(const void**)self = g_bn_type_info_vftable;
    void* name = *(void**)(self + 4);
    if (name != nullptr) std::free(name);
}

// _ptiddata::_token (+0x18 in the VS2003 layout) is the strtok continuation; the batch-1 __getptd block is
// the engine's only thread, so the field lives in that block.
#define BN_PTD_TOKEN 0x18

// CRT init/terminate host seams. A zero entry means "not bound": every seam then takes its default.
struct BnCrtInitEnv {
    int (*get_version)(uint32_t* out);          // GetVersionExA -> {platform, major, minor, build}
    int (*heap_init)(void);
    int (*mt_init)(void);
    void (*rtc_initialize)(void);
    const char* (*command_line)(void);          // GetCommandLineA
    const char* (*environment_strings)(void);   // ___crtGetEnvironmentStringsA
    int (*io_init)(void);
    int (*set_argv)(void);
    int (*set_envp)(void);
    int (*cinit)(int);
    void (*cexit)(void);
    void (*io_term)(void);
    void (*mt_term)(void);
    void (*heap_term)(void);
    int (*locale_init)(void);
    uint32_t (*current_thread_id)(void);
    void (*free_ptd)(void*);
};

static int bn_crt_default_version(uint32_t* out) { out[0] = 2; out[1] = 5; out[2] = 1; out[3] = 2600; return 1; }
static int bn_crt_default_ok(void) { return 1; }
static int bn_crt_default_io_init(void) { return 0; }          // __ioinit returns >= 0 on success
static int bn_crt_default_cinit(int) { return 0; }             // __cinit returns == 0 on success
static const char* bn_crt_default_empty(void) { return ""; }
static uint32_t bn_crt_default_tid(void) { return 1; }          // the engine's only thread
static void bn_crt_default_void(void) {}
static void bn_crt_default_voidptr(void*) {}

static const BnCrtInitEnv g_bn_crt_default = {
    bn_crt_default_version, bn_crt_default_ok,        bn_crt_default_ok,
    bn_crt_default_void,    bn_crt_default_empty,     bn_crt_default_empty,
    bn_crt_default_io_init, bn_crt_default_ok,        bn_crt_default_ok,
    bn_crt_default_cinit,   bn_crt_default_void,      bn_crt_default_void,
    bn_crt_default_void,    bn_crt_default_void,      bn_crt_default_ok,
    bn_crt_default_tid,     bn_crt_default_voidptr,
};

static int g_bn_crt_termination_done;   // 0x1E360 _C_Termination_Done
static int g_bn_crt_attached;           // 0x1E368 __proc_attached
static uint32_t g_bn_crt_version[4];    // 0x1E324..0x1E334, the GetVersionExA words
static void* g_bn_crt_thread_ptd;       // the single per-thread block this engine can own

static int bn_crt_init_apply(uint32_t hinst, int action, const BnCrtInitEnv* env) {
    (void)hinst;
    if (action == 1) {                                  // DLL_PROCESS_ATTACH
        uint32_t ver[4] = {0, 0, 0, 0};
        if (env->get_version == nullptr || !env->get_version(ver)) return 0;
        std::memcpy(g_bn_crt_version, ver, sizeof g_bn_crt_version);
        if (!env->heap_init()) return 0;
        if (!env->mt_init()) { env->heap_term(); return 0; }
        if (env->rtc_initialize) env->rtc_initialize();
        if (env->command_line) (void)env->command_line();
        if (env->environment_strings) (void)env->environment_strings();
        if (env->io_init() >= 0) {
            if (env->set_argv() >= 0 && env->set_envp() >= 0 && env->cinit(0) == 0) {
                ++g_bn_crt_attached;
                return 1;
            }
            env->io_term();
        }
        env->mt_term();
        env->heap_term();
        return 0;
    }
    if (action == 0) {                                  // DLL_PROCESS_DETACH
        if (g_bn_crt_attached <= 0) return 0;
        --g_bn_crt_attached;
        if (g_bn_crt_termination_done == 0) env->cexit();
        env->io_term();
        env->mt_term();
        env->heap_term();
        return 1;
    }
    if (action == 2) {                                  // DLL_THREAD_ATTACH
        void* ptd = std::calloc(1, 0x8C);
        if (ptd == nullptr) return 0;
        if (env->locale_init()) {
            (void)bn_updatetlocinfo();                  // __initptd(ptd, locale): the zeroed block is the model
            *(uint32_t*)((uint8_t*)ptd + 0x04) = 0xFFFFFFFFu;   // _thandle = -1
            *(uint32_t*)ptd = env->current_thread_id();          // _tid
            g_bn_crt_thread_ptd = ptd;
            return 1;
        }
        std::free(ptd);
        return 0;
    }
    if (action == 3) {                                  // DLL_THREAD_DETACH
        if (env->free_ptd) env->free_ptd(nullptr);      // __freeptd(NULL): the current thread's block
        std::free(g_bn_crt_thread_ptd);
        g_bn_crt_thread_ptd = nullptr;
        return 1;
    }
    return 1;
}

// Module DllMain body (FUN_6FF373A0) and the optional DLL callback (DAT_6FF40D04); both are host seams.
static int (*g_bn_module_main)(uint32_t, int, uint32_t);
static int (*g_bn_dll_callback)(uint32_t, int, uint32_t);
// __endthreadex's registered callback (DAT_6FF40D08) and KERNEL32 ExitThread (terminal, host seam).
static void (*g_bn_endthreadex_cb)(void);
static void (*g_bn_exit_thread)(uint32_t);

// CreateThread seam for __beginthreadex: the host owns the actual thread. NULL = no threading available.
struct BnBeginThreadEnv {
    void* (*create_thread)(void* security, uint32_t stack, void* start, void* arg, uint32_t flags,
                           uint32_t* thread_id);
    void (*init_ptd)(void* ptd, void* locale);
    uint32_t (*last_error)(void);
    void (*dos_map_err)(uint32_t);
};
static BnBeginThreadEnv g_bn_begin_thread;

// 1.13c BNClient+0x1AF0 (292 B): _strncpy. Copy at most count bytes from src to dest stopping after the
// NUL; when the source is shorter, pad the rest of count with zeros. The decompile's word-at-a-time loops
// are the compiler's form of the same rule. Returns dest.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1af0(uint32_t* r) {
    char* const dest = (char*)(uintptr_t)r[0];
    const char* src = (const char*)(uintptr_t)r[1];
    size_t count = (size_t)r[2];
    char* d = dest;
    if (count != 0) {
        while (count != 0 && (*d++ = *src++) != '\0') --count;
        if (count != 0) {
            --count;
            while (count != 0) { *d++ = '\0'; --count; }
        }
    }
    r[4] = (uint32_t)(uintptr_t)dest;
}

// 1.13c BNClient+0x1C14 (61 B): type_info::~type_info. Store &vftable into the object, __lock(0xE), free
// the name pointer at +4 when non-NULL, __unlock(0xE). Void.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1c14(uint32_t* r) {
    bn_type_info_destroy((uint8_t*)(uintptr_t)r[0]);
}

// 1.13c BNClient+0x1C51 (9 B): the unlock half of the type_info destructor, FUN_6FF230AE(0xE) = __unlock.
// The engine is single-threaded and carries no CRT lock table, so this is the no-op the call leaves behind.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1c51(uint32_t* r) {
    (void)r;   // lock number 0xE, no state in this engine
}

// 1.13c BNClient+0x1C5A (28 B): type_info's scalar deleting destructor. Run ~type_info(this); when
// (param_1 & 1) != 0 free the object; return this.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1c5a(uint32_t* r) {
    uint8_t* const self = (uint8_t*)(uintptr_t)r[0];
    bn_type_info_destroy(self);
    if ((r[1] & 1) != 0) std::free(self);
    r[4] = (uint32_t)(uintptr_t)self;
}

// 1.13c BNClient+0x1D3E (34 B): _tolower. Fetch the thread data block, refresh its locale info when it is
// not the process locale, then fold the character through ___tolower_mt. ASCII-only in this port (the
// 0x1C76 fast path): 0x41..0x5A map to +0x20, everything else is unchanged.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1d3e(uint32_t* r) {
    (void)bn_getptd();
    void* const locale = bn_updatetlocinfo();
    r[4] = (uint32_t)bn_tolower_mt(locale, (int)r[0]);
}

// 1.13c BNClient+0x1D60 (191 B): _strtok. Build the 256-bit delimiter map from param_2 (the terminating
// NUL is part of the set), start from param_1 or the thread's saved _token when param_1 is NULL, skip the
// delimiters, then scan the token to its delimiter. On a delimiter, NUL it and save the next byte as the
// new _token, returning the token start; at the end of the string save the terminator and return NULL when
// there was no token. Returns the token pointer (0 when there is none).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1d60(uint32_t* r) {
    uint8_t* s = (uint8_t*)(uintptr_t)r[0];
    const uint8_t* del = (const uint8_t*)(uintptr_t)r[1];
    uint8_t map[32];
    std::memset(map, 0, sizeof map);
    uint8_t b;
    do {
        b = *del++;
        map[b >> 3] = (uint8_t)(map[b >> 3] | (1u << (b & 7)));
    } while (b != 0);
    uint8_t* const ptd = (uint8_t*)bn_getptd();
    if (s == nullptr) s = *(uint8_t**)(ptd + BN_PTD_TOKEN);
    uint8_t* p = s;
    while (*p != 0 && (map[*p >> 3] & (1u << (*p & 7))) != 0) ++p;
    uint8_t* q = p;
    for (;;) {
        if (*q == 0) {
            *(uint8_t**)(ptd + BN_PTD_TOKEN) = q;
            r[4] = p != q ? (uint32_t)(uintptr_t)p : 0u;
            return;
        }
        if ((map[*q >> 3] & (1u << (*q & 7))) != 0) {
            *q = 0;
            *(uint8_t**)(ptd + BN_PTD_TOKEN) = q + 1;
            r[4] = (uint32_t)(uintptr_t)p;
            return;
        }
        ++q;
    }
}

// 1.13c BNClient+0x1E1F (58 B): _isdigit. With a multibyte locale the table is bypassed for __isctype_mt;
// the shipped single-byte C table flags 0x30..0x39 with 0x04 and returns that byte. The port's ASCII table
// does the same: 4 for a digit, 0 otherwise.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1e1f(uint32_t* r) {
    const int c = (int)r[0];
    (void)bn_getptd();
    (void)bn_updatetlocinfo();
    r[4] = (c >= 0x30 && c <= 0x39) ? 4u : 0u;
}

// 1.13c BNClient+0x1E59 (48 B): __endthreadex. Call the module callback DAT_6FF40D08 when registered, get
// the thread block (NULL -> __amsg_exit(0x10), terminal), release it, then ExitThread(retval) -- a host
// terminal seam here because wasm has no ExitThread.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1e59(uint32_t* r) {
    if (g_bn_endthreadex_cb != nullptr) g_bn_endthreadex_cb();
    void* const ptd = bn_getptd();
    (void)ptd;   // retail's NULL branch is __amsg_exit(0x10); our __getptd always succeeds
    if (g_bn_exit_thread != nullptr) g_bn_exit_thread(r[0]);
}

// 1.13c BNClient+0x1F1F (139 B): __beginthreadex. NULL start address sets errno = EINVAL (0x16) and returns
// 0. Otherwise allocate the 0x8C thread block, __initptd it, store start/arg for the thread wrapper, hand
// the create to the host (CreateThread seam), return the handle. A failed create maps the last error through
// __dosmaperr and frees the block. Arguments: r[0]=security, r[1]=stack, r[2]=start, r[3]=arg,
// r[4]=init flag, r[5]=&thread id (optional); result in r[6].
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1f1f(uint32_t* r) {
    void* const security = (void*)(uintptr_t)r[0];
    const uint32_t stack = r[1];
    void* const start = (void*)(uintptr_t)r[2];
    void* const arg = (void*)(uintptr_t)r[3];
    const uint32_t flags = r[4];
    uint32_t* const thread_id = (uint32_t*)(uintptr_t)r[5];
    if (start == nullptr) {                              // errno = EINVAL (0x16)
        r[6] = 0;
        return;
    }
    void* const ptd = std::calloc(1, 0x8C);
    if (ptd != nullptr) {
        if (g_bn_begin_thread.init_ptd != nullptr) g_bn_begin_thread.init_ptd(ptd, bn_updatetlocinfo());
        *(uint32_t*)((uint8_t*)ptd + 0x04) = 0xFFFFFFFFu;        // _thandle = -1
        uint32_t id = 0;
        uint32_t* const out_id = thread_id != nullptr ? thread_id : &id;
        void* handle = g_bn_begin_thread.create_thread != nullptr
                           ? g_bn_begin_thread.create_thread(security, stack, start, arg, flags, out_id)
                           : nullptr;
        if (handle != nullptr) {
            r[6] = (uint32_t)(uintptr_t)handle;
            return;
        }
        const uint32_t err = g_bn_begin_thread.last_error != nullptr ? g_bn_begin_thread.last_error() : 0;
        std::free(ptd);
        if (err != 0 && g_bn_begin_thread.dos_map_err != nullptr) g_bn_begin_thread.dos_map_err(err);
    }
    r[6] = 0;
}

// 1.13c BNClient+0x1FB0 (45 B): _strrchr. Scan to the end of the string, then back for the last byte equal
// to (char)_Ch (the terminator included when _Ch is 0). Returns that pointer or NULL.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1fb0(uint32_t* r) {
    const char* s = (const char*)(uintptr_t)r[0];
    const char ch = (char)r[1];
    const char* last = nullptr;
    do {
        if (*s == ch) last = s;
    } while (*s++ != '\0');
    r[4] = (uint32_t)(uintptr_t)last;
}

// 1.13c BNClient+0x1FDD (385 B): __CRT_INIT@12, the CRT's DllMain-style init/terminate dispatcher.
//   1 (PROCESS_ATTACH): GetVersionExA fills the version globals; then __heap_init, __mtinit,
//     __RTC_Initialize, command line + environment strings, __ioinit, __setargv, __setenvp, __cinit(0).
//     Success increments __proc_attached and returns 1; any failure unwinds __ioterm/__mtterm/__heap_term
//     in reverse and returns 0.
//   0 (PROCESS_DETACH): only when __proc_attached > 0. Decrement; if !_C_Termination_Done run __cexit;
//     then __ioterm, __mtterm, __heap_term; return 1 (0 when the module was not attached).
//   2 (THREAD_ATTACH): calloc(1,0x8C), locale init, __initptd with the process locale, _thandle = -1,
//     _tid = GetCurrentThreadId; return 1. Allocation/locale failure frees and returns 0.
//   3 (THREAD_DETACH): __freeptd(NULL), return 1. Any other reason returns 1.
// r[0]=hDllHandle, r[1]=reason; r[2]=optional host seam table (NULL -> built-in defaults); result in r[4].
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1fdd(uint32_t* r) {
    const BnCrtInitEnv* const env =
        r[2] != 0 ? (const BnCrtInitEnv*)(uintptr_t)r[2] : &g_bn_crt_default;
    r[4] = (uint32_t)bn_crt_init_apply(r[0], (int)r[1], env);
}

// 1.13c BNClient+0x215E (208 B): the module's `entry`. result starts 1. PROCESS_DETACH with no attachment
// yields 0. For PROCESS_ATTACH/THREAD_ATTACH the DLL callback (DAT_6FF40D04) runs first and the CRT init is
// applied; a zero from either ends the call with 0. Otherwise the DllMain body FUN_6FF373A0 runs; an
// attach failure triggers the CRT detach, and on PROCESS_DETACH/PROCESS_DETACH-of-thread the CRT runs
// before the callback. Returns the final result.
// r[0]=hinst, r[1]=reason, r[2]=reserved, r[3]=optional host seam table, r[4]=result;
// r[5]/r[6] optionally install the module-main and DLL-callback seams.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_215e(uint32_t* r) {
    if (r[5] != 0) g_bn_module_main = (int (*)(uint32_t, int, uint32_t))(uintptr_t)r[5];
    if (r[6] != 0) g_bn_dll_callback = (int (*)(uint32_t, int, uint32_t))(uintptr_t)r[6];
    const uint32_t hinst = r[0];
    const int reason = (int)r[1];
    const uint32_t reserved = r[2];
    const BnCrtInitEnv* const env =
        r[3] != 0 ? (const BnCrtInitEnv*)(uintptr_t)r[3] : &g_bn_crt_default;
    int result = 1;
    if (reason == 0 && g_bn_crt_attached == 0) {
        result = 0;
    } else {
        bool zeroed = false;
        if (reason == 1 || reason == 2) {
            if (g_bn_dll_callback != nullptr) result = g_bn_dll_callback(hinst, reason, reserved);
            if (result == 0 || bn_crt_init_apply(hinst, reason, env) == 0) zeroed = true;
        }
        if (zeroed) {
            result = 0;
        } else {
            result = g_bn_module_main != nullptr ? g_bn_module_main(hinst, reason, reserved) : 1;
            if (reason == 1 && result == 0) (void)bn_crt_init_apply(hinst, 0, env);
            if (reason == 0 || reason == 3) {
                if (bn_crt_init_apply(hinst, reason, env) == 0) result = 0;
                if (result != 0 && g_bn_dll_callback != nullptr)
                    result = g_bn_dll_callback(hinst, reason, reserved);
            }
        }
    }
    r[4] = (uint32_t)result;
}
