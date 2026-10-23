// src/core/d2cmp/d2cmp_batch_tiny_1018c.cpp -- five tiny 1.13c D2CMP.dll rows transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2cmp/decomp/0x<RVA>.c). Image base 0x6FE10000; every
// RVA below is retail's. Alpha batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no
// vector. They are MSVC 2003 (VC7.1) Release CRT glue: the RTTI scalar deleting destructor, __cexit,
// the two __onexit-side lock thunks, and the CRT lock-table leaf 0x2DD8 all of them end at.
// Host services the decompiles name but src/ does not implement are seams after the retail arguments,
// which keep their order (src/core/realm/mcp_crt_batch_b52.cpp pattern).
// Prior art: D2MOO 1.10f reference/D2MOO/source (no CRT modules); the byte-identical lock leaf is
// src/core/d2common/d2common_batch_tiny_1019i.cpp 0x2E44.
#include <cstdint>
#include <cstddef>

#include <Windows.h>   // LeaveCriticalSection: kernel state, a no-op under wasm

// The lock table behind 0x2DD8: (&DAT_6fe32060)[index * 2], an 8-byte stride over the CRT's
// critical-section pointers. Under the compat shim the section is a no-op, so the entry address is
// not observable; the index is the whole rule (d2common 0x2E44 models the same table the same way).
static uint8_t g_d2cmp_csTable_6fe32060[0x200];

// 1.13c D2CMP+0x2DD8 (21 bytes, 15 callers): release CRT lock-table entry `index`.
// 1.13c D2CMP+0x02dd8
extern "C"
void d2cmp_2dd8(uint32_t index) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(g_d2cmp_csTable_6fe32060 + (size_t)index * 8));
}

// 1.13c D2CMP+0x2A33 (6 bytes, caller 0x2A01 __onexit): the lock-8 release thunk. The decompile's
// callee 0x2782 is itself only `0x2DD8(8)`, so it collapses into the leaf above.
// 1.13c D2CMP+0x2A33
extern "C"
void d2cmp_2a33(void) {
    d2cmp_2dd8(8);
}

// 1.13c D2CMP+0x28F2 (15 bytes, caller 0x2AA2 __CRT_INIT): __cexit -- doexit(0, 0, 1), i.e. run the
// atexit tables, skip __crtExitProcess (param_3 == 1 makes doexit return instead of exiting).
typedef void (*D2CMPDoExit)(uint32_t code, int32_t runAtexit, int32_t returnInsteadOfExit);

// 1.13c D2CMP+0x28F2
extern "C"
void d2cmp_28f2(D2CMPDoExit doexit) {
    doexit(0, 0, 1);
}

// 1.13c D2CMP+0x2A39 (18 bytes, caller 0x27A3 __cinit): _atexit(func) -- register through __onexit
// (0x2A01) and return 0 on success, -1 when it fails.
// 1.13c D2CMP+0x2A39
extern "C"
int32_t d2cmp_2a39(void* func, void* (*onexit)(void*)) {
    void* registered = onexit(func);
    return (registered != nullptr) - 1;
}

// 1.13c D2CMP+0x272D (28 bytes, caller in the RTTI vtable): type_info's scalar deleting destructor.
// Destruct the object (0x26E7, ~type_info), and when bit 0 of the flag is set hand it to the delete
// path (0x1000 -> Ordinal_403(obj, "delete", -1, 0)). Returns its own `this`.
struct D2CMPBatchTiny1018cEnv {
    void  (*destroyTypeInfo)(void* obj);    // 0x26E7, ~type_info
    void  (*freeObject)(void* obj);         // 0x1000, Ordinal_403(obj, "delete", -1, 0)
};

// 1.13c D2CMP+0x272D
extern "C"
void* d2cmp_272d(void* this_, uint32_t flags, const D2CMPBatchTiny1018cEnv* env) {
    env->destroyTypeInfo(this_);
    if ((flags & 1) != 0) env->freeObject(this_);
    return this_;
}

// 1.13c D2CMP+0x1A998 (13 bytes, caller 0x1A85D _raise): the SEH-cleanup fragment `if (frame[-0x1c] != lock)
// _unlock(lock)`. The decompile reads the caller's EBP frame; `frameLockSlot` is that slot's value.
// 1.13c D2CMP+0x1A998
extern "C"
void d2cmp_1a998(int32_t lock, int32_t frameLockSlot) {
    if (frameLockSlot != lock) d2cmp_2dd8((uint32_t)lock);
}
