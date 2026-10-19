// src/core/d2cmp/d2cmp_batch_tiny_1018.cpp -- five UNMAPPED 1.13c D2CMP.dll rows reimplemented from
// the cached Ghidra decompiles (build-wasm/ghidra-d2cmp/decomp/0x<RVA>.c). Alpha batch lane
// (OBJECTIVE v8.3): implementation + marker only, no spec, no vector. Base 0x6FE10000; every RVA
// below is retail's.
//
// Three are MSVC 2003 (VC7.1) Release CRT leaves the linker pulled into the module -- the module's
// operator delete (0x1000), a DeleteCriticalSection wrapper (0x1110), and the two nine-byte lock
// wrappers _unlock(0xE) / _lock(8) -- plus a UCELLIST destructor (0x12C0). Every free reaches Storm
// through the import thunk at 0x8D5A: Storm.dll!#403 SMemFree(ptr, file, line, flags). The CRT lock
// table DAT_6fe32060 is host (kernel) state a no-op behind the Windows compat shim, as in
// src/core/fog/fog_batch_2000.cpp. Prior art: the sibling batch files
// src/core/realm/bnclient_batch_1000.cpp (Storm #403 operator delete) and
// src/core/realm/mcp_tiny_rowex.cpp (the lock-table wrappers).
#include <cstdint>
#include <cstddef>
#include <Windows.h>   // CRITICAL_SECTION, Delete/Enter/LeaveCriticalSection: no-ops under wasm

// Storm.dll!#403 SMemFree(p, file, line, flags), imported through thunk 0x8D5A.
typedef void (*D2CmpSmemFreeFn)(void* p, const char* file, int32_t line, uint32_t flags);

// The RTTI name of the array type 0x12C0 frees, at 0x6FE3417C (literal ".?AUCELLIST@@").
static const char kUcellistType[] = ".?AUCELLIST@@";

// +0x2E060 DAT_6fe32060: the CRT lock table, 8-byte CRITICAL_SECTION stride (the decompile indexes
// it as dwords, hence `idx * 2`). Zero-initialised; slot 0 is unused.
static uint32_t g_d2cmp_lockTable_6fe32060[32];

// LeaveCriticalSection(_locktable[idx]) -- the same read 0x2DD8 makes.
static void d2cmp_leave_cs(uint32_t idx) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(uintptr_t)g_d2cmp_lockTable_6fe32060[idx * 2]);
}

// Host seam for the callee the cached decompile names but src/ does not implement: 0x1580, the
// UCELLIST in-place destructor body (walks two sub-lists at +0x2C, then 0x15D0).
struct D2CmpTiny1018Env {
    void (*releaseUcellist)(void* list);   // 0x1580
    D2CmpSmemFreeFn smemFree;              // Storm #403
};

// 1.13c D2CMP+0x1000 (24 B, caller 0x272D the type_info scalar deleting destructor): operator
// delete. A non-NULL block goes to SMemFree(block, "delete", -1, 0); NULL is ignored.
// 1.13c D2CMP+0x1000
extern "C"
void d2cmp_1000(void* block, const D2CmpTiny1018Env* env) {
    if (block != nullptr) env->smemFree(block, "delete", -1, 0);
}

// 1.13c D2CMP+0x1110 (8 B, callers 0x1AF2A and 0x1B07A): DeleteCriticalSection(this). The
// decompile recovers the this pointer only from EAX, so it is the explicit argument here. Kernel
// state; a no-op under the wasm shim.
// 1.13c D2CMP+0x1110
extern "C"
void d2cmp_1110(CRITICAL_SECTION* section) {
    DeleteCriticalSection(section);
}

// 1.13c D2CMP+0x12C0 (30 B, no callers in the image): UCELLIST destructor teardown. Run the
// in-place destructor body 0x1580, then SMemFree(list, ".?AUCELLIST@@", -2, 0). The pointer is not
// cleared.
// 1.13c D2CMP+0x12C0
extern "C"
void d2cmp_12c0(void* list, const D2CmpTiny1018Env* env) {
    env->releaseUcellist(list);
    env->smemFree(list, kUcellistType, -2, 0);
}

// 1.13c D2CMP+0x2724 (9 B, caller 0x26E7 type_info::~type_info): _unlock(0xE) -- `push 0xE; call
// 0x2DD8; pop ecx; ret`. Releases CRT lock 0xE.
// 1.13c D2CMP+0x2724
extern "C"
void d2cmp_2724(void) {
    d2cmp_leave_cs(0xE);
}

// 1.13c D2CMP+0x2779 (9 B, caller 0x2A01 __onexit): _lock(8) -- `push 8; call 0x2E8D (__lock);
// pop ecx; ret`. Takes CRT lock 8; __lock's lazy initialisation is the same host table.
// 1.13c D2CMP+0x2779
extern "C"
void d2cmp_2779(void) {
    EnterCriticalSection((LPCRITICAL_SECTION)(uintptr_t)g_d2cmp_lockTable_6fe32060[8 * 2]);
}
