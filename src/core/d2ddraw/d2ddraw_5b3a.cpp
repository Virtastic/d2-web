// src/core/d2ddraw/d2ddraw_5b3a.cpp -- 1.13c D2DDraw.dll _isdigit (statically linked CRT). Decompile-derived.
// cdecl (c). With mb_cur_max > 1 the answer is __isctype_mt(loc, c, 4); otherwise the low byte of ctype[c] & 4.
#include "d2ddraw_crt.h"

// 1.13c D2DDraw+0x05B3A
extern "C" uint32_t d2ddraw_isdigit_5b3a(const D2DDrawCrtEnv* e, int32_t c) {
    using namespace d2ddraw_crt;
    const uint8_t* loc = locinfo(e);
    if ((int32_t)rd32(loc + 0x28) > 1) return e->isctype(loc, c, 4);
    return ptr(loc + 0x48)[c * 2] & 4u;
}
