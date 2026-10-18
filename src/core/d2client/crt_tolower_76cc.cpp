// src/core/d2client/crt_tolower_76cc.cpp -- the CRT _tolower wrapper statically linked into
// D2Client.dll (VS2003 Release), from the 1.13c decompile and disassembly. Base 0x6FAB0000.
//
// 6fab76cc: call __getptd (0x72F7)
//           8b 40 64              mov 0x64(%eax),%eax        ; _ptiddata::pthreadlocinfo
//           3b 05 3c b6 b8 6f     cmp 0x6fb8b63c,%eax        ; PTR_DAT, RVA +0xDB63C
//           74 05                 je  +5
//           e8 ..                 call ___updatetlocinfo (0x7283)
//           ff 74 24 04           push 0x4(%esp)             ; the original int argument
//           50                    push %eax                  ; the locale
//           e8 ..                 call ___tolower_mt (0x7604)
//           59 59 c3              pop ecx; pop ecx; ret
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// _ptiddata (VS2003, 0x8C bytes): the thread's pthreadlocinfo is the dword at +0x64. The CRT
// default locale is the dword at +0xDB63C (initial value &__ptlocinfo at +0xDB5E8); the ctype
// rows in d2client_batch_03f82.cpp own that global, this wrapper is its second reader.
extern uint32_t g_default_locale;   // +0x0DB63C  PTR_DAT_6fb8b63c

// __getptd is NAMED in d2client_batch_908f.cpp; the env shape is repeated so the TUs agree at the
// C ABI (function pointers only).
struct D2cGetptdEnv {
    uint32_t (*get_last_error)();
    void     (*set_last_error)(uint32_t);
    void*    (*tls_get_value)(uint32_t);
    int32_t  (*tls_set_value)(uint32_t, void*);
    void*    (*calloc_)(uint32_t, uint32_t);
    uint32_t (*get_current_thread_id)();
    void     (*amsg_exit)(int32_t);
};
extern "C" void* d2_client_72f7(const D2cGetptdEnv* env);

typedef void*    (*D2cUpdatetlocinfoFn)(void);
typedef int32_t (*D2cTolowerMtFn)(const void* self, const void* locale, int32_t c);

// ___tolower_mt (0x7604) is its own function and is a seam here. Retail's
// ECX at the call site is whatever __getptd / ___updatetlocinfo left in it (Ghidra's extraout_ECX,
// `this` is only used by the code-page path), so self is passed as null.
// 1.13c D2Client+0x076CC
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_76cc(int32_t c, const D2cGetptdEnv* env,
                                            D2cUpdatetlocinfoFn updatetlocinfo,
                                            D2cTolowerMtFn tolower_mt) {
    const uint8_t* ptd = (const uint8_t*)d2_client_72f7(env);
    uint32_t locale = *(const uint32_t*)(ptd + 0x64);
    if (locale != g_default_locale) locale = (uint32_t)(uintptr_t)updatetlocinfo();
    return tolower_mt(nullptr, (const void*)(uintptr_t)locale, c);
}
