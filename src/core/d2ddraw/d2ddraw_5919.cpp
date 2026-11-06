// src/core/d2ddraw/d2ddraw_5919.cpp -- 1.13c D2DDraw.dll the CRT's default-ANSI-code-page reader. Decompile-derived.
// cdecl (lcid): GetLocaleInfoA(lcid, LOCALE_IDEFAULTANSICODEPAGE 0x1004, buf, 6) into a 6-byte local (buf[6] is
// zeroed first); failure is -1, otherwise atol(buf). The frame carries the /GS cookie check.
#include "d2ddraw_crt.h"

// 1.13c D2DDraw+0x05919
extern "C" int32_t d2ddraw_locale_info_long_5919(const D2DDrawCrtEnv* e, uint32_t lcid) {
    char buf[7];
    buf[6] = 0;
    if (e->getLocaleInfoA(lcid, 0x1004, buf, 6) == 0) return -1;
    return e->atol(buf);
}
