// src/core/d2ddraw/d2ddraw_5c5e.cpp -- 1.13c D2DDraw.dll ___isctype_mt (statically linked CRT). Decompile-derived.
// cdecl (loc, c, mask). c in -1..255 reads the ctype word table. Otherwise the byte pair {c >> 8, c, 0} (when the
// high byte's table word has the lead-byte bit 0x8000) or {c, 0} goes through ___crtGetStringTypeA (CT_CTYPE1,
// the code page at loc+4, the LCID at loc+0x14, error 1) into the high half of the c argument slot (which keeps c's high half when not written); a zero answer
// returns 0, else that word & mask.
#include "d2ddraw_crt.h"

// 1.13c D2DDraw+0x05C5E
extern "C" uint32_t d2ddraw_isctype_mt_5c5e(const D2DDrawCrtEnv* e, const uint8_t* loc, int32_t c, uint32_t mask) {
    using namespace d2ddraw_crt;
    const uint8_t* tab = ptr(loc + 0x48);
    if ((uint32_t)c + 1u <= 0x100u) { uint16_t w; __builtin_memcpy(&w, tab + c * 2, 2); return w & mask; }
    const uint8_t hi = (uint8_t)(c >> 8), lo = (uint8_t)c;
    uint8_t buf[4];
    int32_t n;
    if (tab[hi * 2 + 1] & 0x80) { buf[0] = hi; buf[1] = lo; buf[2] = 0; n = 2; }
    else { buf[0] = lo; buf[1] = 0; n = 1; }
    uint32_t args[2] = { (uint32_t)c, mask };   // the c and mask argument slots: the word lands in c's high half,
    uint16_t* out = (uint16_t*)((uint8_t*)args + 2);   // as retail passes &[ebp+0xE]; a dword write reaches mask
    if (e->getStringTypeA(1, buf, n, out, rd32(loc + 4), rd32(loc + 0x14), 1) == 0) return 0;
    uint16_t w; __builtin_memcpy(&w, (uint8_t*)args + 2, 2);
    return w & args[1];
}
