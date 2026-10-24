// D2Lang CRT rows (l03stl). Host seams stand in for the Win32/CRT callees.
#include "../crt_freefls.h"
#include <cstdint>

// 1.13c D2Lang+0x1E5F -- 9 bytes: push 0xC; call 0x2215 (LeaveCriticalSection on CRT lock 12).
extern "C" void d2lang_sub_1e5f(void (*leave)(int32_t)) { leave(12); }

// 1.13c D2Lang+0x1F12 -- __freefls@4: frees the per-thread _tiddata's owned buffers, drops the mbcinfo
// (lock 13) and locinfo (lock 12) refcounts, then the block itself.
extern "C" void d2lang_freefls(CrtTidData* t, const CrtFreeflsEnv* env) { crt_freefls_body(t, *env); }
