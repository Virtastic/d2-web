// src/core/d2client/crt_mbtowc_ab10.cpp -- 1.13c D2Client.dll row 0x0AB10: _mbtowc, the MSVC 2003
// Release CRT narrow/wide leaf. Image base 0x6FAB0000. The 43-byte decompile (0xAB10) loads the thread
// data block through 0x072F7 __getptd, reads its locale pointer (_tfpecode), and when that differs from
// the module global locale at +0x0DB63C refreshes it through 0x07283 ___updatetlocinfo, then
// tail-calls 0x0AA50 ___mbtowc_mt with (ptlocinfo, dst, src, srcsize). Nothing else is read.
// The _tfpecode offset inside _ptiddata is a seam: Ghidra exposes only the field name. The body is the
// mirror of 0x0A024 _wctomb (src/core/d2client/crt_wctomb_a024.cpp).
#include <cstdint>
#include <emscripten.h>

typedef void* (*D2CGetPtdFn)(void);
typedef void* (*D2CUpdatetlocinfoFn)(void);
typedef int32_t (*D2CMbtowcMtFn)(int32_t ptlocinfo, uint16_t* dst, const uint8_t* src,
                                 uint32_t src_size);

struct D2ClientMbtowcEnv {
    D2CGetPtdFn getptd;                    // 0x072F7 __getptd
    D2CUpdatetlocinfoFn updatetlocinfo;    // 0x07283 ___updatetlocinfo
    D2CMbtowcMtFn mbtowc_mt;               // 0x0AA50 ___mbtowc_mt
    void* global_ptlocinfo;                // +0x0DB63C PTR_DAT_6fb8b63c, the module locale
    uint32_t ptlocinfo_offset;             // _tfpecode offset in retail's _ptiddata
};

// 1.13c D2Client+0x0AB10
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_ab10(uint16_t* dst, const char* src, uint32_t src_size,
                                            const D2ClientMbtowcEnv* env) {
    void* ptlocinfo = *(void**)((uint8_t*)env->getptd() + env->ptlocinfo_offset);
    if (ptlocinfo != env->global_ptlocinfo) ptlocinfo = env->updatetlocinfo();
    return env->mbtowc_mt((int32_t)(uintptr_t)ptlocinfo, dst, (const uint8_t*)src, src_size);
}
