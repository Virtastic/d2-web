// src/core/d2client/d2client_wctomb_mt.cpp -- 1.13c D2Client.dll ___wctomb_mt, reimplemented from
// build-wasm/ghidra-d2client/decomp/0x9FC4.c (alpha lane). 96 bytes, sole caller 0x0A024 _wctomb.
// param_1 is the CRT locale (ptlocinfo): +0x04 code page, +0x14 mbctype word (0 = C locale), +0x28
// output size limit. The kernel32 import (no PE import table in wasm) is a host seam AFTER the retail
// arguments; the error path's __getptd callee (0x0579C) is already recovered in
// src/core/d2client/d2client_batch_4857.cpp and is called directly. The body is identical to D2Gfx's
// ___wctomb_mt (src/core/d2gfx/d2gfx_wctomb_mt.cpp).
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x0579C: &__getptd()->_terrno (recovered).
extern "C" int32_t* d2_client_579c(void);

struct D2ClientWctombMtEnv {
    // IAT WideCharToMultiByte: returns bytes written, 0 on failure; used_default set when substituted.
    int32_t (*wide_char_to_multi_byte)(uint32_t codepage, uint32_t flags, const uint16_t* wch,
                                       int32_t wchlen, char* out, int32_t outlen,
                                       const char* defchar, int32_t* used_default);
};

// 1.13c D2Client+0x09FC4
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_9fc4(uint32_t locale, char* mbch, uint16_t wch,
                                            const D2ClientWctombMtEnv* env) {
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
    *d2_client_579c() = 0x2a;                             // EILSEQ
    return -1;
}
