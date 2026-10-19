// src/core/d2cmp/d2cmp_batch_tiny_1018b.cpp -- five 1.13c D2CMP.dll CRT helpers transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2cmp/decomp/0x<RVA>.c) and checked against the retail
// bytes (retail/D2/D2SE/CORES/1.13c/D2CMP.dll, image base 0x6FE10000). Alpha batch lane (OBJECTIVE
// v8.3): implementation + marker only, no spec, no vector.
//
// 0x280D (doexit) is its own function, so _exit/__exit reach it through a seam instead of a second
// implementation here. 0x2DD8 is _unlock's LeaveCriticalSection wrapper (not a row here).
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2CMP (export map only; the CRT starts are stubs).
#include <cstdint>
#include <cstddef>
#include <Windows.h>   // CRITICAL_SECTION no-ops under wasm

// 0x6FE32060: CRT _locktable, a pointer plus its debug dword every 8 bytes; _lock/_unlock index the
// pair as [i * 2]. Lock 8 is the one this batch drops.
static uint32_t g_d2cmp_locktable_6fe32060[18];

// Callee 0x280D (doexit, its own row): (exit code, fast path, dying), the retail call shape.
extern "C" void d2cmp_280d(uint32_t code, int32_t fast, int32_t dying);

// 1.13c D2CMP+0x02782 (9 bytes): _unlock(8) -- leave the critical section registered in
// _locktable[8]. Only caller is 0x2A33 (__onexit's unlock); under wasm the section is a no-op.
// 1.13c D2CMP+0x02782
extern "C"
void d2cmp_2782(void) {
    LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(
        static_cast<uintptr_t>(g_d2cmp_locktable_6fe32060[8 * 2])));
}

// 1.13c D2CMP+0x0278B (24 bytes): __initterm -- call every non-null function pointer in [begin, end).
// Retail passes begin in EAX and end on the stack (cdecl); the two are explicit here.
// 1.13c D2CMP+0x0278B
extern "C"
void d2cmp_278b(void** begin, void** end) {
    for (void** p = begin; p < end; ++p) {
        if (*p != nullptr) reinterpret_cast<void (*)()>(*p)();
    }
}

// 1.13c D2CMP+0x028BC (14 bytes): doexit's SEH tail (the decompile's EBP+0x10 is doexit's third
// parameter, EDI is zero there): a nonzero value drops lock 8, zero returns without unlocking.
// Callers besides 0x280D: none.
// 1.13c D2CMP+0x028BC
extern "C"
void d2cmp_28bc(uint32_t param_3) {
    if (param_3 != 0) {
        LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(
            static_cast<uintptr_t>(g_d2cmp_locktable_6fe32060[8 * 2])));
    }
}

// 1.13c D2CMP+0x028D0 (17 bytes): _exit(code) -- doexit(code, 0, 0): run the exit tables, then end
// the process with the code.
// 1.13c D2CMP+0x028D0
extern "C"
void d2cmp_28d0(uint32_t code) {
    d2cmp_280d(code, 0, 0);
}

// 1.13c D2CMP+0x028E1 (17 bytes): __exit(code) -- doexit(code, 1, 0): skip the _onexit walk that
// doexit does for its fast==0 path, then end the process with the code.
// 1.13c D2CMP+0x028E1
extern "C"
void d2cmp_28e1(uint32_t code) {
    d2cmp_280d(code, 1, 0);
}
