// src/core/d2common/d2common_crt_b24.cpp -- six 1.13c D2Common.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2common/decomp/0x<RVA>.c (alpha lane, stage A). Base 0x6FD50000;
// every RVA below is retail's.
//
// All six are MSVC 2003 Release CRT glue the linker pulled into the module: the lock-table wrappers
// around lock 8, the exit wrapper's lock-release block, and _exit / __exit / __initterm. Their
// observable is a host operation (a kernel critical section, a process exit, a table call), so each
// host-touching part is a seam: the critical-section calls go through the Windows compat shim (no-ops
// under wasm, as in src/core/d2client/d2client_batch_3664.cpp), and the exit wrappers terminate the
// wasm process with the same code (as src/core/realm/mcp_exit.cpp does).
#include <cstdint>
#include <cstdlib>
#include "d2common_crt_exit.h"

#include "d2common_crt.h"
#include <Windows.h>   // Enter/LeaveCriticalSection: host (kernel) state, a no-op under wasm

// +0x9E9048 DAT_6fde9048: the CRT lock table, 8-byte CRITICAL_SECTION stride; each entry is the
// critical-section pointer the wrappers below act on and 0x2EF9 (__lock) lazily creates.
static uint8_t g_csTable[0x200];

static void enter_cs(uint32_t nIdx) {
    EnterCriticalSection((LPCRITICAL_SECTION)(g_csTable + (size_t)nIdx * 8));
}

static void leave_cs(uint32_t nIdx) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(g_csTable + (size_t)nIdx * 8));
}

// 1.13c D2Common+0x19B6
// Nine-byte wrapper: __lock(8) -- acquire lock-table entry 8. In this single-threaded engine the
// critical section is host state behind the compat shim, so the index is the whole rule.
void D2Common_19B6_Lock8(void)
{
    enter_cs(8);
}

// 1.13c D2Common+0x19BF
// The matching nine-byte wrapper: _unlock(8) -- release lock-table entry 8 (0x2E44 is the
// LeaveCriticalSection leaf). Same rule, opposite direction.
void D2Common_19BF_Unlock8(void)
{
    leave_cs(8);
}

// 1.13c D2Common+0x19C8
// The CRT's __initterm: walk [begin, end) in ascending dword order and call every non-NULL entry with
// no arguments. The private CRT convention passes `begin` in EAX and `end` as the stack argument, so
// the decompile shows one parameter; here both bounds are explicit. A retail entry is a module RVA,
// not a wasm address, so the call is a host seam (`invoke`), the d2_client_351f pattern.
void D2Common_19C8_InitTerm(uint32_t* pBegin, uint32_t* pEnd, void (*invoke)(uint32_t nEntry))
{
    for (uint32_t* p = pBegin; p < pEnd; ++p)
        if (*p != 0) invoke(*p);
}

// 1.13c D2Common+0x1AF9
// The lock-release block inside doexit (its operands are doexit's frame at [EBP+0x10] and the caller's
// EDI, not an argument list): when the two differ, _unlock(8). Both operands belong to doexit, so they
// are taken as arguments, as in the D2Client twin (0x3650).
void D2Common_1AF9_DoExitUnlock(uint32_t nStack10, uint32_t nEdi)
{
    if (nStack10 != nEdi) leave_cs(8);
}

// 1.13c D2Common+0x1B0D
// _exit(code) = doexit(code, quick=0, returning=0): lock 8, the onexit list, both initterm tables,
// then terminate the process and never return. Delegate to the module-owned shutdown state.
extern "C" void D2Common_1B0D_Exit(int32_t nCode)
{
    D2Common_1A4A_DoExit(static_cast<uint32_t>(nCode), 0, 0);
}

// (D2Common+0x1B1E; marker lives in d2common_n69.cpp)
// __exit(code) = doexit(code, quick=1, returning=0): the quick branch skips the onexit walk, still
// runs the terminator table, then terminates through the same module shutdown state.
void D2Common_1B1E_QuickExit(int32_t nCode)
{
    D2Common_1A4A_DoExit(static_cast<uint32_t>(nCode), 1, 0);
}

// 1.13c D2Common+0x422D
// __lock_file2(index, file): stream slots below 0x14 live in the static lock table, so take table lock
// index+0x10 (0x2EF9 __lock, here enter_cs); otherwise the FILE carries its own critical section at +0x20.
void D2Common_422D_LockFile2(int32_t nIndex, void* pFile)
{
    if (nIndex < 0x14) enter_cs((uint32_t)nIndex + 0x10);
    else EnterCriticalSection((LPCRITICAL_SECTION)((uint8_t*)pFile + 0x20));
}

// 1.13c D2Common+0x2EF9
// __lock(index): ensure lock-table entry `index` exists (lazy create via 0x2E59 __mtinitlocknum, fatal
// 0x2D73 __amsg_exit on failure) then enter it. Our table is statically backed, so creation is a no-op.
void D2Common_2EF9_Lock(uint32_t nIdx)
{
    d2common_lock_2ef9(static_cast<int32_t>(nIdx));
}
