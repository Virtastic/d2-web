// 1.13c D2Common+0x07442 _wctomb and +0x073E2 ___wctomb_mt (VS2003 CRT), from the cached decompiles
// (alpha lane). Same bodies as D2Client 0x0A024/0x09FC4. Locale (ptlocinfo): +0x04 code page, +0x14
// mbctype word (0 = C locale), +0x28 output size limit. Imports/CRT callees are env seams after the
// retail arguments; _tfpecode's offset inside _ptiddata is a seam (Ghidra exposes only the name).
#include <cstdint>
#include <emscripten.h>

struct D2CommonWctombEnv {
    void* (*getptd)(void);                 // 0x03475 __getptd
    void* (*updatetlocinfo)(void);         // 0x03401 ___updatetlocinfo
    int32_t* (*errno_ptr)(void);           // 0x044BC _errno
    int32_t (*wide_char_to_multi_byte)(uint32_t cp, uint32_t flags, const uint16_t* wch, int32_t wchlen,
                                       char* out, int32_t outlen, const char* defchar, int32_t* used_default);
    void* global_ptlocinfo;                // PTR_DAT_6fde91c4
    uint32_t ptlocinfo_offset;             // _tfpecode in _ptiddata
};

// 1.13c D2Common+0x073E2
extern "C" EMSCRIPTEN_KEEPALIVE int32_t D2Common_73E2_WctombMt(uint32_t locale, char* mbch, uint16_t wch,
                                                               const D2CommonWctombEnv* env) {
    if (mbch == nullptr) return 0;
    if (*(uint32_t*)(uintptr_t)(locale + 0x14) == 0) {
        if (wch < 0x100) { *mbch = (char)wch; return 1; }
    } else {
        int32_t used_default = 0;
        int32_t n = env->wide_char_to_multi_byte(*(uint32_t*)(uintptr_t)(locale + 4), 0, &wch, 1, mbch,
                                                 *(int32_t*)(uintptr_t)(locale + 0x28), nullptr, &used_default);
        if (n != 0 && used_default == 0) return n;
    }
    *env->errno_ptr() = 0x2a;  // EILSEQ
    return -1;
}

// 1.13c D2Common+0x07442
extern "C" EMSCRIPTEN_KEEPALIVE int32_t D2Common_7442_Wctomb(char* mbch, uint16_t wch, const D2CommonWctombEnv* env) {
    void* loc = *(void**)((uint8_t*)env->getptd() + env->ptlocinfo_offset);
    if (loc != env->global_ptlocinfo) loc = env->updatetlocinfo();
    return D2Common_73E2_WctombMt((uint32_t)(uintptr_t)loc, mbch, wch, env);
}
