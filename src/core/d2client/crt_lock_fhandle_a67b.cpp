// src/core/d2client/crt_lock_fhandle_a67b.cpp -- one UNMAPPED 1.13c D2Client.dll alpha-lane row
// reimplemented from build-wasm/ghidra-d2client/decomp/0xA67B.c (image base 0x6FAB0000).
//
// __lock_fhandle is MSVC 2003 Release CRT glue the linker pulled into the DLL.
// Its observable is host state (a kernel critical section), so the CRT operations it reaches are seams
// after the retail arguments, which keep their order (the d2client_batch_1e70.cpp pattern).
#include <cstdint>
#include <cstddef>
#include <emscripten.h>
#include <Windows.h>   // EnterCriticalSection: host (kernel) state, a no-op under wasm

// +0x1239A0  DAT_6fbd39a0: the CRT file-descriptor table (owned by d2client_batch_03f82.cpp).
// A handle selects block handle >> 5, then record (handle & 0x1F) * 0x24: lockinitflag +0x8,
// CRITICAL_SECTION +0xC.
extern uint32_t g_pioinfo[0x20];

// 0x47AB _unlock(idx), already recovered in d2client_batch_3664.cpp.
extern "C" void d2_client_47ab(uint32_t idx);

struct D2ClientLockFhandleEnv {
    void    (*lock)(int32_t locknum);            // 0x4860 __lock
    int32_t (*init_cs)(void* cs, uint32_t spin); // 0x908F ___crtInitCritSecAndSpinCount
};

// 1.13c D2Client+0x0A67B (148 B, callers 0x5AFF, 0xA8B9, 0xA73D, 0x86EC, 0x60E0, 0x8383):
// __lock_fhandle. Resolve the handle's ioinfo record. When its lockinitflag (+0x8) is still zero, take
// CRT lock 10, initialise the record's section (+0xC) with 4000 spins if still unset, bump the flag,
// and release lock 10; then enter the section and return 1. Retail's only failure arm (the initialiser
// returning 0) unwinds through __local_unwind2, which runs the same _unlock(10) finally and returns 0.
// ponytail: the tree's 0x908F returns void and the wasm host has no failing initialiser, so that arm is
// unreachable; add it if 0x908F ever reports failure.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_a67b(int32_t handle,
                                            const D2ClientLockFhandleEnv* env) {
    uint8_t* const pio = (uint8_t*)(uintptr_t)(g_pioinfo[(uint32_t)handle >> 5] +
                                               ((uint32_t)handle & 0x1F) * 0x24);
    int32_t* const lockinitflag = (int32_t*)(pio + 0x8);
    void* const cs = pio + 0xC;
    if (*lockinitflag == 0) {
        env->lock(10);
        if (*lockinitflag == 0) {
            env->init_cs(cs, 4000);
            *lockinitflag += 1;
        }
        d2_client_47ab(10);   // 0xA712: _unlock(10), retail's SEH finally
    }
    EnterCriticalSection((LPCRITICAL_SECTION)cs);
    return 1;
}
