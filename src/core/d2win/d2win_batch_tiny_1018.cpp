// src/core/d2win/d2win_batch_tiny_1018.cpp -- five UNMAPPED 1.13c D2Win.dll rows transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2win/decomp/0x<RVA>.c) and checked against the retail
// bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base 0x6F8E0000). Alpha batch lane (OBJECTIVE
// v8.3): implementation + marker only, no spec, no vector.
//
// All five are VC7.1 Release CRT leaves around the module's lock table at 0x6F8FF050, the same shape as
// src/core/gfx/d2ddraw_batch_10da.cpp. Prior art: that file (__lock/__unlock leaves and the shared
// table) and src/core/realm/bnclient_batch_2558.cpp (__lock/__unlock bookkeeping).
#include <cstdint>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION: host (kernel) state, a no-op under the wasm compat shim

// Retail's CRT lock table at 0x6F8FF050: 0x24 slots of 8 bytes, the first dword the critical-section
// pointer (filled by __mtinitlocknum, RVA 0x23AB) and the second its static-init flag. Shared with the
// __lock (0x244B) and __mtinitlocknum rows when they are ported; 32-bit pointer fields keep the retail
// encoding, as in d2win_batch_10030.cpp.
extern "C" uint32_t d2win_g_6f8ff050[0x24 * 2] = {0};   // definition: no other TU provides it (link fix, 2026-10-18)

// One static CRITICAL_SECTION per slot stands in for __mtinitlocknum's allocation, so a lock is always
// initialised (retail's __amsg_exit(0x11) path cannot be reached with these compile-time lock numbers).
static CRITICAL_SECTION g_d2win_cs[0x24];

// Modelled from the cached decompile of D2Win+0x244B (__lock), a row of its own not ported here; the
// 0x10A7 thunk enters lock 8 through it. First use fills the slot, then EnterCriticalSection.
static void d2win_lock(int32_t lock_num) {
    if (d2win_g_6f8ff050[lock_num * 2] == 0)
        d2win_g_6f8ff050[lock_num * 2] = (uint32_t)(uintptr_t)&g_d2win_cs[lock_num];
    EnterCriticalSection((LPCRITICAL_SECTION)(uintptr_t)d2win_g_6f8ff050[lock_num * 2]);
}

// 1.13c D2Win+0x2396 (21 B, __unlock): LeaveCriticalSection on the slot the lock number indexes. The
// table entry is loaded, not checked -- retail leaves it unchecked too (a port that reports a miss here
// would hide a lock pairing bug). Returns nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_2396(int32_t lock_num) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(uintptr_t)d2win_g_6f8ff050[lock_num * 2]);
}

// 1.13c D2Win+0x10A7 (9 B, _lockexit): push 8, __lock, pop. Called by __onexit (0x2524).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_10A7(void) { d2win_lock(8); }

// 1.13c D2Win+0x10B0 (9 B, _unlockexit): push 8, __unlock, pop. Called by 0x2556.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_10B0(void) { d2win_sub_2396(8); }

// 1.13c D2Win+0x11EA (14 B): doexit's shared epilogue fragment -- if the third doexit argument
// (EBP+0x10, compared against EDI=0) is non-zero, __unlock(8), then return. Reached by `call` from
// 0x113B and from doexit's own fall-through at 0x11E9, so it is a standalone entry here with the
// third argument made explicit; the exit path itself (0x113B) is a separate row.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_11EA(int32_t param_3) {
    if (param_3 != 0) d2win_sub_2396(8);
}

// 1.13c D2Win+0x2442 (9 B): push 0xA, __unlock, pop. Called by __mtinitlocknum (0x23AB).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_2442(void) { d2win_sub_2396(10); }
