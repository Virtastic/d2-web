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
#include <cstdlib>
#include <unistd.h>
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

// CRT exit state: 0x6FE34640 _C_Exit_Done-style flag, 0x6FE3463C retcaller byte, 0x6FE34644 dying,
// 0x6FF14FB0/0x6FF14FAC the _onexit table begin/end, 0x6FE32030/0x6FE3203C the two terminator
// tables (xp / xt). Tables are linker-built host state, empty under wasm.
static uint32_t g_d2cmp_6fe34640, g_d2cmp_6fe34644;
static uint8_t g_d2cmp_6fe3463c;
static void** g_d2cmp_onexit_begin;   // DAT_6ff14fb0
static void** g_d2cmp_onexit_end;     // DAT_6ff14fac
static void* g_d2cmp_xp_6fe32030[1];
static void* g_d2cmp_xt_6fe3203c[1];

// 1.13c D2CMP+0x0280D (181 bytes): _doexit(code, fast, retcaller). Lock 8; if already dying,
// TerminateProcess; set the exit flags; unless fast, drain the _onexit table backwards and run the
// first terminator table; always run the second; unlock (0x28BC); a nonzero retcaller returns,
// else mark dying and end the process via ___crtExitProcess (0x2749, a host no-op here).
// 1.13c D2CMP+0x0280D
extern "C"
void d2cmp_280d(uint32_t code, int32_t fast, int32_t retcaller) {
    EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(
        static_cast<uintptr_t>(g_d2cmp_locktable_6fe32060[8 * 2])));
    if (g_d2cmp_6fe34644 == 1) _exit((int)code);   // TerminateProcess(GetCurrentProcess(), code)
    g_d2cmp_6fe34640 = 1;
    g_d2cmp_6fe3463c = static_cast<uint8_t>(retcaller);
    if (fast == 0) {
        if (g_d2cmp_onexit_begin != nullptr) {
            while (--g_d2cmp_onexit_end, g_d2cmp_onexit_begin <= g_d2cmp_onexit_end) {
                if (*g_d2cmp_onexit_end != nullptr)
                    reinterpret_cast<void (*)()>(*g_d2cmp_onexit_end)();
            }
        }
        d2cmp_278b(g_d2cmp_xp_6fe32030, g_d2cmp_xp_6fe32030);
    }
    d2cmp_278b(g_d2cmp_xt_6fe3203c, g_d2cmp_xt_6fe3203c);
    d2cmp_28bc(static_cast<uint32_t>(retcaller));
    if (retcaller != 0) return;
    g_d2cmp_6fe34644 = 1;
    // ___crtExitProcess(code): process end, host-side.
}
