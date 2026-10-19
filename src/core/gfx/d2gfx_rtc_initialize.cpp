// src/core/gfx/d2gfx_rtc_initialize.cpp -- one UNMAPPED 1.13c D2Gfx.dll row reimplemented from the
// cached decompile build-wasm/ghidra-d2gfx/decomp/0x1AA2.c (alpha lane: no spec, no vector).
// Retail base 0x6FA80000; the module global below is named at its retail RVA.
//
// MSVC 2003 (VC7.1) Release CRT glue pulled into the DLL: the runtime-check
// initialiser walk, called once from __CRT_INIT at 0x1686. The 61 bytes are mostly the injected
// __SEH_prolog / __SEH_epilog glue (callees 0x1B67, 0x1B2C), not program data.
#include <emscripten.h>

// +0xE7B8 (`DAT_6fa8e7b8`): the initialiser table. In the shipped image the walk's begin and end are
// the same address, so it holds no entries and no hook ever runs; the bounds are kept as one empty
// range rather than collapsed so the rule stays visible.
static void (*g_rtc_table[1])(void);   // +0xE7B8, both bounds

// 1.13c D2Gfx+0x01AA2 (__RTC_Initialize): call every non-null initialiser in table order. Pure in
// its two bounds; returns nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2gfx_1aa2(void) {
    void (**begin)(void) = g_rtc_table;
    void (**end)(void) = g_rtc_table;
    for (void (**p)(void) = begin; p < end; ++p)
        if (*p != nullptr) (*p)();
}
