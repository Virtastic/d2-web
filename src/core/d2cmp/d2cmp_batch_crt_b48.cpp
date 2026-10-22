// src/core/d2cmp/d2cmp_batch_crt_b48.cpp -- 1.13c D2CMP.dll MSVC 2003 CRT rows, from the cached
// Ghidra decompiles (build-wasm/ghidra-d2cmp/decomp/0x<RVA>.c). Alpha batch lane: marker only.
// Not ported (no faithful wasm64 form): 0x2F30/0x2F6B __SEH_prolog/_epilog (x86 stack-frame
// intrinsics), 0x1F30/0x0BFD0 (register-passed 32-bit pointer lists), 0x4EC6/0x4EF1 (small-block
// heap over 32-bit region records; malloc/free are the host's).
#include <cstdint>
#include <cstdlib>
#include <cstddef>

extern "C" void d2cmp_2dd8(uint32_t index);   // 0x2DD8 lock release
extern "C" void d2cmp_28e1(uint32_t code);       // 0x28E1 __exit
extern "C" void* d2cmp_malloc(size_t n);      // 0x3DD2
// Host seams after retail's callees (weak: absent = skipped).
extern "C" __attribute__((weak)) void d2cmp_4abc(int msg);   // 0x4ABC _NMSG_WRITE
extern "C" __attribute__((weak)) void d2cmp_4c34(void);      // 0x4C34 __FF_MSGBANNER
extern "C" __attribute__((weak)) int  d2cmp_callnewh(size_t); // 0x6315 __callnewh

// 1.13c D2CMP+0x2F11 (9 bytes): release CRT lock 4 (the heap lock).
// 1.13c D2CMP+0x2F11
extern "C" void d2cmp_2f11(void) { d2cmp_2dd8(4); }

// Per-thread CRT block (0x8C bytes in retail); only the fields __getptd initialises are modelled.
struct D2CMPPtd { uint32_t tid; int32_t thandle; int32_t terrno; uint32_t holdrand; uint8_t rest[0x8c - 16]; };
static thread_local D2CMPPtd g_ptd;
static thread_local bool g_ptdInit;

// 1.13c D2CMP+0x3E1D (113 bytes): __getptd -- this thread's CRT block, created on first use
// (_holdrand = 1, _thandle = -1). GetLastError is preserved around it in retail; no-op here.
// 1.13c D2CMP+0x3E1D
extern "C" D2CMPPtd* d2cmp_getptd(void) {
    if (!g_ptdInit) { g_ptd = D2CMPPtd{}; g_ptd.holdrand = 1; g_ptd.thandle = -1; g_ptdInit = true; }
    return &g_ptd;
}

// 1.13c D2CMP+0x4D08 (9 bytes): _errno() -- &__getptd()->_terrno.
// 1.13c D2CMP+0x4D08
extern "C" int32_t* d2cmp_4d08(void) { return &d2cmp_getptd()->terrno; }

// 1.13c D2CMP+0x2D07 (51 bytes): __amsg_exit -- banner (when the app type asks), message, exit(255).
// The two app-type globals (6fe34654/58) are the console/GUI selector; the GUI path (neither) skips the banner.
// 1.13c D2CMP+0x2D07
extern "C" void d2cmp_2d07(int msg) {
    if (d2cmp_4c34) d2cmp_4c34();
    if (d2cmp_4abc) d2cmp_4abc(msg);
    d2cmp_28e1(0xff);
}

// 1.13c D2CMP+0x3DA6 (44 bytes): __nh_malloc -- retry loop through the new-handler.
// 1.13c D2CMP+0x3DA6
extern "C" void* d2cmp_3da6(size_t size, int nhFlag) {
    if (size >= 0xffffffe1u) return nullptr;
    for (;;) {
        void* p = d2cmp_malloc(size);
        if (p) return p;
        if (!nhFlag || !d2cmp_callnewh || !d2cmp_callnewh(size)) return nullptr;
    }
}

// 1.13c D2CMP+0x2DED (151 bytes): __mtinitlocknum -- lazily create CRT lock `n` (0x18-byte section).
// Critical sections are no-ops here, so the entry is the 0x18-byte block itself. Returns 1/0, errno=12 on OOM.
static void* g_lockTab[64];
// 1.13c D2CMP+0x2DED
extern "C" int d2cmp_2ded(int lockNum) {
    void** slot = &g_lockTab[lockNum];
    if (*slot) return 1;
    void* cs = malloc(0x18);
    if (!cs) { *d2cmp_4d08() = 12; return 0; }
    *slot = cs;
    return 1;
}
