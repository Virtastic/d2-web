// src/core/d2gfx/d2gfx_batch_b45.cpp -- 1.13c D2Gfx.dll alpha-lane batch (b45): five tiny CRT leaves,
// reimplemented from build-wasm/ghidra-d2gfx/decomp/0x{4345,5D6B,5390,37BD,39B6}.c. Image base
// 0x6FA80000; every global is named at its retail RVA. Kernel/CRT seams (a function pointer, EAX/EBP
// that a C signature cannot carry) are host arguments AFTER the retail ones, which keep their order.
#include <cstdint>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION / Initialize/LeaveCriticalSection: host state, no-ops under wasm

// +0x10048 DAT_6fa90048: the CRT lock table the 0x19BC `_unlock` indexes as (&table)[idx * 2]. Modelled
// at retail width (0x40 pointer slots); the compat shim's LeaveCriticalSection is invisible, as it is
// for the sibling model in d2gfx_crt_unlock.cpp.
static uint32_t g_lock_table[0x40];
static void d2gfx_unlock(uint32_t idx) { LeaveCriticalSection((LPCRITICAL_SECTION)(uintptr_t)g_lock_table[idx * 2]); }

extern "C"
// 1.13c D2Gfx+0x04345 (27 B): __callnewh(size). Load the CRT's installed new-handler (+0x110C8); when it
// is non-null call it with the size and answer 1 only if it returns non-zero, else 0. Same body as
// D2MCPClient 0x401F and BNClient 0x49AE. No recovered D2Gfx row writes the slot, so its value is the
// seam, passed after the retail argument.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2gfx_4345(uint32_t size, int32_t (*handler)(uint32_t)) {
    if (handler == nullptr) return 0;
    return handler(size) != 0 ? 1 : 0;
}

extern "C"
// 1.13c D2Gfx+0x05390 (9 B): ___updatetlocinfo's (0x535E) `_unlock(0xc)` leaf -- put the critical
// section at lock-table slot 12, void, no branch.
void d2_fn_d2gfx_5390(void) { d2gfx_unlock(0xc); }

extern "C"
// 1.13c D2Gfx+0x05D6B (9 B): __lock_fhandle's (0x5CD4) `_unlock(10)` leaf -- release lock-table slot 10
// after the critical section has been initialised, void, no branch.
void d2_fn_d2gfx_5d6b(void) { d2gfx_unlock(10); }

extern "C"
// 1.13c D2Gfx+0x037BD (16 B, ___crtInitCritSecNoSpinCount@8): retail's fallback initialiser --
// InitializeCriticalSection(cs); return 1. The stdcall @8 name carries a spin count at [esp+8] that the
// body never reads.
int32_t d2_fn_d2gfx_37bd(void* cs, uint32_t spin) {
    (void)spin;
    InitializeCriticalSection((LPCRITICAL_SECTION)cs);
    return 1;
}

// +0x1042C/0x10430/0x10434 DAT_6fa9042c/30/34: the __NLG notifier's saved EAX, stack argument and EBP.
static uint32_t g_nlg_eax, g_nlg_param, g_nlg_ebp;

extern "C"
// 1.13c D2Gfx+0x039B6 (24 B): __NLG_Notify, the cdecl sibling of __NLG_Notify1 (0x39AD, which takes its
// argument in ECX) -- store the stack argument at +0x10430, EAX at +0x1042C and EBP at +0x10434 for the
// notifier. EAX/EBP cannot ride a C signature, so they are the seam, after the retail stack argument.
void d2_fn_d2gfx_39b6(uint32_t param, uint32_t eax, uint32_t ebp) {
    g_nlg_param = param;
    g_nlg_eax = eax;
    g_nlg_ebp = ebp;
}
