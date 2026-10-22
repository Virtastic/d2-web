// src/core/d2multi/d2multi_batch_b65.cpp -- D2Multi.dll CRT leaves (batch b65-mul).
// Not ported: __heap_init/__heap_term (HeapCreate/VirtualFree), __XcptFilter (UnhandledExceptionFilter),
// FUN_448C (_NMSG_WRITE: WriteFile/MessageBox), __chkstk (stack probe, no-op on wasm).
#include <initializer_list>
#include <cstdint>
#include <cstdarg>
#include <cstdio>

// 1.13c D2Multi+0x1000  _sprintf: vsprintf into _Dest, returns chars written
int d2multi_b65_sprintf(char* dest, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vsprintf(dest, fmt, ap);
    va_end(ap);
    return n;
}

// 1.13c D2Multi+0x138D  __cinit: run the pre-init table [a0,a1) until one returns nonzero, then the
// C++ init table [b0,b1). Retail also calls DAT_6fa0b4b8(param_1) first and _atexit(LAB_340d); both
// are passed in (null = skip).
typedef int (*b65_initfn)(void);
int d2multi_b65_cinit(void (*fpinit)(int), int param_1, void (*atexit_reg)(void),
                      b65_initfn* a0, b65_initfn* a1, void (**b0)(void), void (**b1)(void)) {
    if (fpinit) fpinit(param_1);
    int r = 0;
    for (b65_initfn* p = a0; p < a1; ++p) {
        if (r != 0) return r;
        if (*p) r = (*p)();
    }
    if (r != 0) return r;
    if (atexit_reg) atexit_reg();
    for (void (**p)(void) = b0; p < b1; ++p)
        if (*p) (*p)();
    return 0;
}

// doexit seam: doexit(code, quick, retcaller), retail D2Multi+0x13F7
typedef void (*b65_doexit_fn)(int, int, int);

// 1.13c D2Multi+0x14BA  _exit
void d2multi_b65_exit(b65_doexit_fn doexit, int code) { doexit(code, 0, 0); }

// 1.13c D2Multi+0x14CB  __exit
void d2multi_b65_exit_quick(b65_doexit_fn doexit, unsigned code) { doexit((int)code, 1, 0); }

// 1.13c D2Multi+0x14DC  __cexit
void d2multi_b65_cexit(b65_doexit_fn doexit) { doexit(0, 0, 1); }

// 1.13c D2Multi+0x196E  ___tolower_mt. `this` (ECX) = unused here; pti = locale struct:
// +0x04 codepage, +0x14 lcid, +0x24 flag, +0x28 mb_cur_max, +0x48 ctype table (uint16 per char).
// isctype(c, mask) and lcmap(lcid, buf, len, cp, out[2]) are the ___isctype_mt / FUN_35FE seams.
typedef unsigned (*b65_isctype_fn)(unsigned c, int mask);
typedef unsigned (*b65_lcmap_fn)(uint32_t lcid, const char* in, unsigned len, uint32_t cp, uint8_t* out);
unsigned d2multi_b65_tolower_mt(const uint8_t* pti, unsigned c, b65_isctype_fn isctype, b65_lcmap_fn lcmap) {
    auto i32 = [&](int o) { return *(const int32_t*)(pti + o); };
    if (i32(0x14) == 0 || (i32(0x24) != 0 && c < 0x80)) {
        if ((int)c > 0x40 && (int)c < 0x5b) return c + 0x20;
        return c;
    }
    const uint8_t* ct = *(const uint8_t* const*)(pti + 0x48);
    if (c < 0x100) {
        unsigned u = i32(0x28) < 2 ? (ct[c * 2] & 1u) : isctype(c, 1);
        if (u == 0) return c;
    }
    char in[4] = {};
    unsigned len;
    if ((ct[1 + ((c >> 8) & 0xff) * 2] & 0x80) == 0) { in[0] = (char)c; len = 1; }
    else { in[0] = (char)(c >> 8); in[1] = (char)c; len = 2; }
    uint8_t out[4] = {};
    unsigned r = lcmap((uint32_t)i32(0x14), in, len, (uint32_t)i32(0x4), out);
    if (r == 0) return c;
    if (r != 1) return (unsigned)(out[0] << 8 | out[1]);
    return out[0];
}

// 1.13c D2Multi+0x41E2  ___heap_select: heap kind (1 = SBH-less, 3 = small-block heap).
// DAT_6fa0ba84 = platform id, DAT_6fa0ba90 = major version, passed in.
int d2multi_b65_heap_select(int osplatform, int osmajor) {
    return (osplatform == 2 && osmajor > 4) ? 1 : 3;
}
