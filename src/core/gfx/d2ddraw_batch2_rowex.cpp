// src/core/gfx/d2ddraw_batch2_rowex.cpp -- UNMAPPED 1.13c D2DDraw.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2ddraw/decomp/0x<RVA>.c (alpha lane, no spec/vector yet). Retail
// base 0x6F8C0000; every RVA below is retail's.
//
// All six rows are MSVC 2003 (VC7.1) Release CRT glue the linker pulled into the DLL
//: the SEH prolog/epilog frame helpers and the CRT lock-table trio
// (__mtinitlocks, __mtdeletelocks, __mtinitlocknum, plus the unlock fragment at 0x1A17). The lock
// table itself (0xE038, 8-byte record per slot) lives in d2ddraw_batch_rowex.cpp and is reached
// through its shared accessors; retail's kernels (Enter/Leave/DeleteCriticalSection, the spin-count
// initialiser) are host seams and no-ops under wasm, as in the batch-1 file.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cerrno>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION: host (kernel) state, a no-op under wasm

extern "C" {
uint32_t* d2ddraw_lock_table(void);   // 0xE038, the CRT lock table (batch 1)
void d2ddraw_locknum(int idx);        // __lock, batch 1's model
void d2ddraw_unlocknum(int idx);      // __unlock, batch 1's model
}

// Retail's +0xEF40 block of pre-built critical sections: 0x24 records of 0x18 bytes, handed out by
// __mtinitlocks to the slots flagged static (table[i*2+1] == 1).
static uint32_t g_static_cs[0x24 * 0x18 / 4];

// Retail's ExceptionList (FS:[0]). There is no SEH in wasm; the frame walk is modelled against this
// module word so the prolog/epilog pair still round-trips.
static void* g_exception_list;

// RVA 0x3C27 ___crtInitCritSecAndSpinCount: InitializeCriticalSectionAndSpinCount from kernel32,
// else the no-spin fallback. Neither exists under wasm, and the compat CRITICAL_SECTION cannot fail,
// so the initialiser always reports success (retail's non-zero).
static int crt_init_cs(uint32_t* cs, uint32_t spin_count) {
    (void)spin_count;
    InitializeCriticalSection((LPCRITICAL_SECTION)cs);
    return 1;
}

// 1.13c D2DDraw+0x01824 (__SEH_prolog): frame_size is the local frame (param_2), frame points at
// auStack_18[0]. Saves the caller's EBX, ESI, EDI and return address at frame - frame_size + 0xC/8/4/0,
// then points ExceptionList at the frame's local_8 slot (frame + 0x10, above auStack_18[4]).
// param_1 (the handler) is registered separately and is not read by this body.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_seh_prolog(uint32_t handler, int frame_size, uint32_t* frame,
                                             uint32_t ebx, uint32_t esi, uint32_t edi, uint32_t retaddr) {
    (void)handler;
    uint8_t* base = (uint8_t*)frame - (size_t)frame_size;
    *(uint32_t*)(base + 0xC) = ebx;
    *(uint32_t*)(base + 0x8) = esi;
    *(uint32_t*)(base + 0x4) = edi;
    *(uint32_t*)(base + 0x0) = retaddr;
    g_exception_list = (void*)((uint8_t*)frame + 0x10);
}

// 1.13c D2DDraw+0x0185F (__SEH_epilog): ExceptionList = EBP[-4] (the next outer frame's list), then
// the return address is restored to [EBP]. Both operands are the caller's frame, so both are seams.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_seh_epilog(uint32_t* ebp, uint32_t retaddr) {
    g_exception_list = (void*)(uintptr_t)ebp[-4];
    ebp[0] = retaddr;
}

// 1.13c D2DDraw+0x01979 (__mtinitlocks): for each of the 0x24 lock slots whose flag word (+4) is 1,
// hand it the next never-used 0x18-byte critical section from the static block and initialise it with
// spin count 4000; a failed initialise clears the slot pointer and returns 0. Otherwise 1. The static
// block advances only for flagged slots.
extern "C"
EMSCRIPTEN_KEEPALIVE int d2ddraw_mtinitlocks(void) {
    uint32_t* table = d2ddraw_lock_table();
    uint8_t* next = (uint8_t*)g_static_cs;
    for (int i = 0; i < 0x24; ++i) {
        if (table[i * 2 + 1] == 1) {
            table[i * 2] = (uint32_t)(uintptr_t)next;
            next += 0x18;
            if (crt_init_cs((uint32_t*)(uintptr_t)table[i * 2], 4000) == 0) {
                table[i * 2] = 0;
                return 0;
            }
        }
    }
    return 1;
}

// 1.13c D2DDraw+0x019C2 (__mtdeletelocks): two walks of the 0x24 lock slots (0xE038..0xE158). First:
// a heap lock (pointer set, flag != 1) is deleted, freed and its pointer cleared. Second: a static
// lock (pointer set, flag == 1) is deleted only -- its storage is the module block, not the heap.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_mtdeletelocks(void) {
    uint32_t* table = d2ddraw_lock_table();
    uint32_t* end = table + 0x24 * 2;
    for (uint32_t* p = table; p < end; p += 2) {
        uint32_t cs = p[0];
        if (cs != 0 && p[1] != 1) {
            DeleteCriticalSection((LPCRITICAL_SECTION)(uintptr_t)cs);
            std::free((void*)(uintptr_t)cs);
            p[0] = 0;
        }
    }
    for (uint32_t* p = table; p < end; p += 2) {
        if (p[0] != 0 && p[1] == 1) DeleteCriticalSection((LPCRITICAL_SECTION)(uintptr_t)p[0]);
    }
}

// 1.13c D2DDraw+0x01A17: __unlock fragment -- LeaveCriticalSection(table[locknum * 2]), the slot's
// CRITICAL_SECTION pointer.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_sub_1A17(int locknum) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(uintptr_t)d2ddraw_lock_table()[locknum * 2]);
}

// 1.13c D2DDraw+0x01A2C (__mtinitlocknum): ensure lock slot `locknum` exists. An already-set slot
// returns 1. Otherwise malloc(0x18); a failed allocation sets errno 0xC and returns 0. With the
// memory in hand take lock 10 and re-test the slot: still clear -> initialise with spin 4000 and
// store it (initialise failure frees, sets errno 0xC, unwinds lock 10 and returns 0); already set by
// a racing caller -> free ours. Either way drop lock 10 and return 1.
extern "C"
EMSCRIPTEN_KEEPALIVE int d2ddraw_mtinitlocknum(int locknum) {
    uint32_t* slot = d2ddraw_lock_table() + (size_t)locknum * 2;
    if (*slot != 0) return 1;
    void* mem = std::malloc(0x18);
    if (mem == nullptr) {
        errno = 0xC;                 // retail: *__getptd()->_terrno = 0xC, returned by RVA 0x3CB2
        return 0;
    }
    d2ddraw_locknum(10);
    if (*slot == 0) {
        if (crt_init_cs((uint32_t*)mem, 4000) == 0) {
            std::free(mem);
            errno = 0xC;
            d2ddraw_unlocknum(10);   // retail: __local_unwind2 then the __unlock fragment
            return 0;
        }
        *slot = (uint32_t)(uintptr_t)mem;
    } else {
        std::free(mem);
    }
    d2ddraw_unlocknum(10);
    return 1;
}
