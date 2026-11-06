// src/core/d2ddraw/d2ddraw_crt.h -- the seams of the 1.13c D2DDraw.dll statically linked CRT character helpers
// (0x5891 atol, 0x5919 the locale-default-code-page reader, 0x5B3A isdigit, 0x5C5E __isctype_mt). Decompile-derived.
// The thread data (0x1BBE __getptd) holds its locale at +0x64; a locale other than the module's current one
// (DAT 0xE484) is refreshed through 0x3E4C __updatetlocinfo. Locale words: +0x04 code page, +0x14 LCID,
// +0x28 mb_cur_max, +0x48 the ctype word table (indexed by c, c in -1..255).
#pragma once
#include <cstdint>

struct D2DDrawCrtEnv {
    const uint8_t* (*getptd)();                                          // 0x1BBE
    const uint8_t* (*updatetlocinfo)();                                  // 0x3E4C
    const uint8_t* curLocinfo;                                           // DAT 0xE484
    uint32_t (*isctype)(const uint8_t* loc, int32_t c, uint32_t mask);   // 0x5C5E (cdecl)
    int32_t  (*getStringTypeA)(uint32_t infoType, const uint8_t* src, int32_t n, uint16_t* out,
                               uint32_t codePage, uint32_t lcid, uint32_t error);   // 0x4F88 (cdecl)
    int32_t  (*getLocaleInfoA)(uint32_t lcid, uint32_t type, char* buf, int32_t n); // KERNEL32 GetLocaleInfoA
    int32_t  (*atol)(const char* s);                                     // 0x5891 (cdecl)
};

extern "C" {
int32_t  d2ddraw_atol_5891(const D2DDrawCrtEnv* e, const char* s);
int32_t  d2ddraw_locale_info_long_5919(const D2DDrawCrtEnv* e, uint32_t lcid);
uint32_t d2ddraw_isdigit_5b3a(const D2DDrawCrtEnv* e, int32_t c);
uint32_t d2ddraw_isctype_mt_5c5e(const D2DDrawCrtEnv* e, const uint8_t* loc, int32_t c, uint32_t mask);
}

namespace d2ddraw_crt {
inline uint32_t rd32(const uint8_t* p) { uint32_t v; __builtin_memcpy(&v, p, 4); return v; }
inline const uint8_t* ptr(const uint8_t* p) { return (const uint8_t*)(uintptr_t)rd32(p); }
inline const uint8_t* locinfo(const D2DDrawCrtEnv* e) {   // __getptd()->ptlocinfo, refreshed when stale
    const uint8_t* loc = ptr(e->getptd() + 0x64);
    if (loc != e->curLocinfo) loc = e->updatetlocinfo();
    return loc;
}
}
