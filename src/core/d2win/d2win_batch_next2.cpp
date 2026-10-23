// D2Win MSVC2003 CRT atexit / locale-tolower wrappers (batch next-2). Pure logic; CRT state passed in.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

struct D2WinOnexitTab { uint32_t** end; uint32_t** begin; };

// 1.13c D2Win+0x24FC  ___onexitinit: allocate 0x80-byte (32 entry) table, both ptrs at it, first slot 0. 0 ok / 0x18 on OOM.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_onexitinit_24fc(uint32_t** tab_ptrs /*[2]: begin,end*/) {
    uint32_t* t = (uint32_t*)malloc(0x80);
    tab_ptrs[0] = (uint32_t*)t;
    if (!t) return 0x18;
    *t = 0;
    tab_ptrs[1] = t;
    return 0;
}

// 1.13c D2Win+0x2524  __onexit: lock(0x10A7), __onexit_lk(0x2633) result, unlock(0x2556); returns the lk result.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_onexit_2524(uint32_t (*onexit_lk)(uint32_t), uint32_t fn) { return onexit_lk(fn); }

// 1.13c D2Win+0x255C  _atexit: 0 if __onexit accepted fn, else -1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_atexit_255c(uint32_t onexit_result) { return (onexit_result != 0) - 1; }

// 1.13c D2Win+0x256E  __RTC_Initialize: the init-pointer range is empty (start == end), so it calls nothing.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_rtc_initialize_256e(void) {}

// 1.13c D2Win+0x2815  _tolower: take thread locinfo (refresh via ___updatetlocinfo if != global), then ___tolower_mt.
extern "C" uint32_t d2win_tolower_mt_274d(const void* loc, uint32_t c, const void* env);
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_tolower_2815(const void* tloc, const void* global_loc, const void* (*update)(void), uint32_t c, const void* env) {
    const void* l = tloc != global_loc ? update() : tloc;
    return d2win_tolower_mt_274d(l, c, env);
}
