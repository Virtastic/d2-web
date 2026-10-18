// src/core/realm/bnclient_batch_612b.cpp -- one UNMAPPED 1.13c BNClient.dll alpha-lane row (Stage A:
// implementation + marker only, no spec, no vector), transcribed from
// build-wasm/ghidra-bnclient/decomp/0x612B.c (image base 0x6FF20000).
//
// Row: ___wctomb_mt 0x612B, MSVC 2003 (VC7.1) Release CRT. Same body and _ptiddata offsets as D2Gfx
// 0x41DB (src/core/d2gfx/d2gfx_wctomb_mt.cpp): +0x04 code page, +0x14 mbctype word, +0x28 output size,
// +0x08 _terrno. WideCharToMultiByte and the errno slot are seams.
#include <cstdint>
#include <emscripten.h>

struct BnWctombMtEnv {
    int32_t (*wide_char_to_multi_byte)(uint32_t codepage, uint32_t flags, const uint16_t* wch,
                                       int32_t wchlen, char* out, int32_t outlen,
                                       const char* defchar, int32_t* used_default);
    int32_t* (*ptd_errno)(void);   // 0x037CD __getptd: address of the thread's CRT errno
};

// 1.13c BNClient+0x0612B (96 B, caller 0x618B): ___wctomb_mt(locale, mbch, wch). A null destination is
// the stateless-encoding query and answers 0. In the C locale (mbctype word zero) a code unit below
// 0x100 is copied as one byte. Otherwise WideCharToMultiByte the code page (flag 0) into the buffer
// with the locale's size limit; success without a substituted default char returns the count. Anything
// else sets _terrno = 0x2A (EILSEQ) and returns -1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_wctomb_mt(uint32_t locale, char* mbch, uint16_t wch,
                                          const BnWctombMtEnv* env) {
    if (mbch == nullptr) return 0;
    if (*(uint32_t*)(uintptr_t)(locale + 0x14) == 0) {
        if (wch < 0x100) { *mbch = (char)wch; return 1; }
    } else {
        int32_t used_default = 0;
        int32_t n = env->wide_char_to_multi_byte(*(uint32_t*)(uintptr_t)(locale + 4), 0, &wch, 1,
                                                 mbch, *(int32_t*)(uintptr_t)(locale + 0x28),
                                                 nullptr, &used_default);
        if (n != 0 && used_default == 0) return n;
    }
    *env->ptd_errno() = 0x2a;   // EILSEQ
    return -1;
}
