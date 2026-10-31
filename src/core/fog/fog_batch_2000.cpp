// src/core/fog/fog_batch_2000.cpp -- five 1.13c Fog.dll rows reimplemented from the decompiles in
// build-wasm/ghidra-fog/decomp/0x<RVA>.c (alpha lane, stage A). Base 0x6FF50000; every RVA below is
// retail's.
//
// Three are nine-byte CRT lock leaves: each calls _unlock(4) -- LeaveCriticalSection on lock-table
// entry 4 (Fog+0x3C3F, DAT_6ff7e210 + 4*8) -- and returns nothing. The lock table is host (kernel)
// state, a no-op behind the Windows compat shim, as in src/core/d2client/d2client_batch_3664.cpp.
// Two are the C-locale character classifiers _isalpha / _isdigit.
#include <cstdint>

#include <Windows.h>   // CRITICAL_SECTION, LeaveCriticalSection: host state, a no-op under wasm

// +0x2E210 DAT_6ff7e210: the CRT lock table, 8-byte CRITICAL_SECTION stride (the decompile indexes
// it as dwords, hence `param_1 * 2`).
extern uint8_t g_fog_csTable[0x400];  // one lock table for the DLL (Fog+0x2E210), defined in fog_batch_tiny_1018c.cpp

static void fog_unlock4(void) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(g_fog_csTable + (size_t)4 * 8));
}

// 1.13c Fog+0x2000
// Nine-byte leaf used by _free: _unlock(4). The index is the whole rule.
extern "C"
void fog_sub_2000(void) {
    fog_unlock4();
}

// 1.13c Fog+0x2090
// The same nine-byte leaf, used by __heap_alloc: _unlock(4).
extern "C"
void fog_sub_2090(void) {
    fog_unlock4();
}

// 1.13c Fog+0x223F
// The same nine-byte leaf, used by the realloc path FUN_6ff520d7: _unlock(4).
extern "C"
void fog_sub_223F(void) {
    fog_unlock4();
}

// __getptd()->_tfpecode is the process default locale (PTR_DAT_6ff7e0d4), so ___updatetlocinfo is
// not called; its category-1 refcount is 1, so the single-byte lc_ctype table branch runs instead
// of ___isctype_mt. The shipped C-locale table is the ASCII class set: 'A'..'Z' hold
// _UPPER|_ALPHA (0x101), 'a'..'z' _LOWER|_ALPHA (0x102), '0'..'9' _DIGIT (0x04), everything else 0.
// The entry for EOF (-1) precedes the table and is 0; inputs outside [0, 255] index past the table
// in retail, so only that range and EOF are modelled.

// 1.13c Fog+0x2289
// _isalpha: table[c] & 0x103. Returns 0x101, 0x102 or 0.
extern "C"
int fog_isalpha(int c) {
    if (c >= 'A' && c <= 'Z') return 0x101;
    if (c >= 'a' && c <= 'z') return 0x102;
    return 0;
}

// 1.13c Fog+0x22C8
// _isdigit: table[c] & 4. Returns 4 for '0'..'9', 0 otherwise.
extern "C"
int fog_isdigit(int c) {
    return (c >= '0' && c <= '9') ? 4 : 0;
}
