// src/core/d2ddraw/d2ddraw_5891.cpp -- 1.13c D2DDraw.dll _atol (statically linked CRT). Decompile-derived.
// cdecl (s). Skips space-class bytes (ctype & 8, through __isctype_mt when mb_cur_max > 1; the locale is read once),
// one optional sign, then decimal digits with 32-bit wraparound; '-' negates.
#include "d2ddraw_crt.h"

// 1.13c D2DDraw+0x05891
extern "C" int32_t d2ddraw_atol_5891(const D2DDrawCrtEnv* e, const char* s) {
    using namespace d2ddraw_crt;
    const uint8_t* loc = locinfo(e);
    const uint8_t* p = (const uint8_t*)s;
    for (;;) {
        const uint32_t ch = *p;
        const uint32_t sp = (int32_t)rd32(loc + 0x28) > 1 ? e->isctype(loc, (int32_t)ch, 8) : (ptr(loc + 0x48)[ch * 2] & 8u);
        if (sp == 0) break;
        ++p;
    }
    const uint32_t sign = *p++;
    uint32_t ch = sign;
    if (sign == '-' || sign == '+') ch = *p++;
    uint32_t v = 0;
    while (ch >= '0' && ch <= '9') { v = v * 10u + (ch - '0'); ch = *p++; }
    return sign == '-' ? (int32_t)(0u - v) : (int32_t)v;
}
