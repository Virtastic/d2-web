// src/core/d2gfx/d2gfx_init_crit_sec.cpp -- 1.13c D2Gfx.dll ___crtInitCritSecAndSpinCount,
// reimplemented from build-wasm/ghidra-d2gfx/decomp/0x37CD.c (alpha lane). Image base 0x6FA80000;
// every global is named at its retail RVA. The kernel32 seams (GetModuleHandleA/GetProcAddress) are
// host arguments AFTER the retail ones, which keep their order (the d2client_batch_908f.cpp pattern).
#include <cstdint>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION / InitializeCriticalSection: host (kernel) state, a no-op under wasm

// --- retail module state, all zero-initialised ------------------------------------------------
static uint32_t g_os_platform;        // +0x10DA4  DAT_6fa90da4: OS/platform selector (1 = NT, else Win9x)
typedef int32_t (*D2GfxInitCritSecFn)(void*, uint32_t);
static D2GfxInitCritSecFn g_init_spin; // +0x11070 DAT_6fa91070: resolved initialiser (kernel32 or 0x37BD)

// RVA 0x37BD ___crtInitCritSecNoSpinCount@8, retail's fallback: InitializeCriticalSection(cs); return 1.
// It is a separate UNMAPPED function; modelled here only as this row's fallback target.
static int32_t d2gfx_init_crit_sec_no_spin(void* cs, uint32_t spin) {
    (void)spin;
    InitializeCriticalSection((LPCRITICAL_SECTION)cs);
    return 1;
}

struct D2GfxInitCritSecEnv {
    uint32_t (*get_module_handle)(const char* name);                       // IAT GetModuleHandleA
    D2GfxInitCritSecFn (*get_proc_address)(uint32_t mod, const char* name); // IAT GetProcAddress
};

// 1.13c D2Gfx+0x037CD (103 B): resolve InitializeCriticalSectionAndSpinCount once. On NT (platform
// word 1) retail never asks kernel32; otherwise a null GetModuleHandleA or GetProcAddress leaves the
// no-spin thunk 0x37BD in the slot. Then call the slot with (cs, spin).
// 1.13c D2Gfx+0x037CD
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2gfx_37cd(void* cs, uint32_t spin, const D2GfxInitCritSecEnv* env) {
    if (g_init_spin == nullptr) {
        D2GfxInitCritSecFn resolved = nullptr;
        if (g_os_platform != 1) {
            uint32_t mod = env->get_module_handle("kernel32.dll");
            if (mod != 0)
                resolved = env->get_proc_address(mod, "InitializeCriticalSectionAndSpinCount");
        }
        g_init_spin = resolved != nullptr ? resolved : d2gfx_init_crit_sec_no_spin;
    }
    g_init_spin(cs, spin);
}
