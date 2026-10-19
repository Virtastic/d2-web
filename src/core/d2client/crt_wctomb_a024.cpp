// src/core/d2client/crt_wctomb_a024.cpp -- 1.13c D2Client.dll row 0x0A024: _wctomb, the MSVC 2003
// Release CRT narrow/wide leaf. Image base 0x6FAB0000. The 39-byte decompile (0xA024) loads the thread
// data block through 0x072F7 __getptd, reads its locale pointer (_tfpecode), and when that differs from
// the module global locale at +0x0DB63C refreshes it through 0x07283 ___updatetlocinfo, then tail-calls
// 0x09FC4 ___wctomb_mt with (ptlocinfo, mbch, wch). Nothing else is read.
// The _tfpecode offset inside _ptiddata is a seam: Ghidra exposes only the field name (the bnclient
// 0x618B row pins the same offset the same way).
#include <cstdint>
#include <emscripten.h>

typedef void* (*D2CGetPtdFn)(void);
typedef void* (*D2CUpdatetlocinfoFn)(void);
typedef int32_t (*D2CWctombMtFn)(void* ptlocinfo, char* mbch, uint16_t wch);

struct D2ClientWctombEnv {
    D2CGetPtdFn getptd;                    // 0x072F7 __getptd
    D2CUpdatetlocinfoFn updatetlocinfo;    // 0x07283 ___updatetlocinfo
    D2CWctombMtFn wctomb_mt;               // 0x09FC4 ___wctomb_mt
    void* global_ptlocinfo;                // +0x0DB63C PTR_DAT_6fb8b63c, the module locale
    uint32_t ptlocinfo_offset;             // _tfpecode offset in retail's _ptiddata
};

// 1.13c D2Client+0x0A024
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_a024(char* mbch, uint16_t wch,
                                            const D2ClientWctombEnv* env) {
    void* ptlocinfo = *(void**)((uint8_t*)env->getptd() + env->ptlocinfo_offset);
    if (ptlocinfo != env->global_ptlocinfo) ptlocinfo = env->updatetlocinfo();
    return env->wctomb_mt(ptlocinfo, mbch, wch);
}
