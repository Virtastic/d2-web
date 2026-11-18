// src/core/d2gfx/d2gfx_wctomb_mt.cpp -- 1.13c D2Gfx.dll ___wctomb_mt, reimplemented from
// build-wasm/ghidra-d2gfx/decomp/0x41DB.c (alpha lane). Image base 0x6FA80000. param_1 is the CRT
// locale (ptlocinfo): +0x04 code page, +0x14 mbctype word (0 = C locale), +0x28 output size limit.
// The kernel32 import (no PE import table in wasm) and the unrecovered __getptd helper are host seams
// AFTER the retail arguments, which keep their order (the d2gfx_sbh_free_block pattern).
#include <cstdint>
#include <emscripten.h>

struct D2GfxWctombMtEnv {
    // IAT WideCharToMultiByte: returns bytes written, 0 on failure; used_default set when substituted.
    int32_t (*wide_char_to_multi_byte)(uint32_t codepage, uint32_t flags, const uint16_t* wch,
                                       int32_t wchlen, char* out, int32_t outlen,
                                       const char* defchar, int32_t* used_default);
    int32_t* (*ptd_errno)(void);   // 0x03858 __getptd: address of the thread's CRT errno
};

// 1.13c D2Gfx+0x041DB
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2gfx_wctomb_mt(uint32_t locale, char* mbch, uint16_t wch,
                                              const D2GfxWctombMtEnv* env) {
    if (mbch == nullptr) return 0;                        // stateful-encoding query; C locale: 0
    if (*(uint32_t*)(uintptr_t)(locale + 0x14) == 0) {    // mbctype word zero: the C locale
        if (wch < 0x100) { *mbch = (char)wch; return 1; }
    } else {                                              // DBCS locale: convert through the code page
        int32_t used_default = 0;
        int32_t n = env->wide_char_to_multi_byte(*(uint32_t*)(uintptr_t)(locale + 4), 0, &wch, 1,
                                                 mbch, *(int32_t*)(uintptr_t)(locale + 0x28),
                                                 nullptr, &used_default);
        if (n != 0 && used_default == 0) return n;
    }
    *env->ptd_errno() = 0x2a;                             // EILSEQ
    return -1;
}
