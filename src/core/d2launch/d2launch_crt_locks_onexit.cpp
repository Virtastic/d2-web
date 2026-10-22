// src/core/d2launch/d2launch_crt_locks_onexit.cpp -- MSVC 7.1 CRT lock table and atexit leaves of
// 1.13c D2Launch.dll (batch, implementation + markers only). Win32 / CRT callees are host seams.
#include <cstdint>
#include <cstddef>

// DAT_6fa60050: 0x24 pairs { critical section*, preallocated flag }. Pair [1]==1 means the section
// lives in the static block DAT_6fa64e40 (0x18 bytes each) and is never heap-freed.
struct LaunchLockEntry { void* cs; int32_t isStatic; };
struct LaunchLockEnv {
    int32_t (*initCs)(void* cs, uint32_t spin);   // ___crtInitCritSecAndSpinCount (0x5F04)
    void (*enterCs)(void* cs);                    // EnterCriticalSection
    void (*leaveCs)(void* cs);                    // LeaveCriticalSection
    void (*deleteCs)(void* cs);                   // DeleteCriticalSection
    void* (*alloc)(size_t n);                     // _malloc (0x59F8)
    void (*release)(void* p);                     // _free (0x43B8)
    void (*amsgExit)(int32_t code);               // __amsg_exit (0x1B6B)
    void (*setErrnoNoMem)(void);                  // *FUN_3AFA() = 0xC
};

// 1.13c D2Launch+0x033DB -- __mtinitlocks
extern "C" int32_t d2launch_mtinitlocks(LaunchLockEntry* tbl, uint8_t* staticBlock, const LaunchLockEnv& e) {
    for (int32_t i = 0; i < 0x24; ++i) {
        if (tbl[i].isStatic == 1) {
            tbl[i].cs = staticBlock;
            staticBlock += 0x18;
            if (e.initCs(tbl[i].cs, 4000) == 0) { tbl[i].cs = nullptr; return 0; }
        }
    }
    return 1;
}

// 1.13c D2Launch+0x03424 -- __mtdeletelocks
extern "C" void d2launch_mtdeletelocks(LaunchLockEntry* tbl, const LaunchLockEnv& e) {
    for (int32_t i = 0; i < 0x24; ++i)
        if (tbl[i].cs != nullptr && tbl[i].isStatic != 1) {
            e.deleteCs(tbl[i].cs); e.release(tbl[i].cs); tbl[i].cs = nullptr;
        }
    for (int32_t i = 0; i < 0x24; ++i)
        if (tbl[i].cs != nullptr && tbl[i].isStatic == 1) e.deleteCs(tbl[i].cs);
}

// 1.13c D2Launch+0x03479 -- unlock(n): LeaveCriticalSection(table[n])
extern "C" void d2launch_sub_3479(LaunchLockEntry* tbl, int32_t n, const LaunchLockEnv& e) { e.leaveCs(tbl[n].cs); }

// 1.13c D2Launch+0x03525 -- unlock(10) thunk
extern "C" void d2launch_sub_3525(LaunchLockEntry* tbl, const LaunchLockEnv& e) { d2launch_sub_3479(tbl, 10, e); }

// 1.13c D2Launch+0x0352E -- __lock
extern "C" int32_t d2launch_mtinitlocknum(LaunchLockEntry* tbl, int32_t n, const LaunchLockEnv& e);
extern "C" void d2launch_lock(LaunchLockEntry* tbl, int32_t n, const LaunchLockEnv& e) {
    if (tbl[n].cs == nullptr && d2launch_mtinitlocknum(tbl, n, e) == 0) e.amsgExit(0x11);
    e.enterCs(tbl[n].cs);
}

// 1.13c D2Launch+0x0348E -- __mtinitlocknum (SEH only guards the unlock of lock 10 on the error path)
extern "C" int32_t d2launch_mtinitlocknum(LaunchLockEntry* tbl, int32_t n, const LaunchLockEnv& e) {
    LaunchLockEntry* slot = &tbl[n];
    if (slot->cs != nullptr) return 1;
    void* mem = e.alloc(0x18);
    if (mem == nullptr) { e.setErrnoNoMem(); return 0; }
    d2launch_lock(tbl, 10, e);
    if (slot->cs == nullptr) {
        if (e.initCs(mem, 4000) == 0) {
            e.release(mem); e.setErrnoNoMem();
            d2launch_sub_3525(tbl, e);   // __local_unwind2 runs the finally block
            return 0;
        }
        slot->cs = mem;
    } else {
        e.release(mem);
    }
    d2launch_sub_3525(tbl, e);
    return 1;
}

// DAT_6fa680d4 / DAT_6fa680d0: atexit table base and next-free slot.
struct LaunchOnexit { uint32_t* base; uint32_t* next; };
struct LaunchOnexitEnv {
    size_t (*msize)(void* p);                  // __msize (0x6230)
    uint32_t* (*realloc_)(void* p, size_t n);  // FUN_607E (_realloc)
    void* (*alloc)(size_t n);                  // _malloc
    void (*lock8)(void);                       // 0x1504
    void (*unlock8)(void);                     // 0x3639 -> 0x150D
};

// 1.13c D2Launch+0x0355F -- __onexit_lk (function pointer arrives in EDI)
extern "C" void d2launch_onexit_lk(LaunchOnexit* t, uint32_t func, const LaunchOnexitEnv& e) {
    size_t sz = e.msize(t->base);
    if (sz < (size_t)((uint8_t*)t->next + 4 - (uint8_t*)t->base)) {
        size_t grow = sz < 0x800 ? sz : 0x800;
        uint32_t* p = e.realloc_(t->base, grow + sz);
        if (p == nullptr) {
            p = e.realloc_(t->base, sz + 0x10);
            if (p == nullptr) return;
        }
        t->next = p + (t->next - t->base);
        t->base = p;
    }
    *t->next++ = func;
}

// 1.13c D2Launch+0x035DF -- ___onexitinit
extern "C" uint32_t d2launch_onexitinit(LaunchOnexit* t, const LaunchOnexitEnv& e) {
    t->base = (uint32_t*)e.alloc(0x80);
    if (t->base == nullptr) return 0x18;
    *t->base = 0;
    t->next = t->base;
    return 0;
}

// 1.13c D2Launch+0x03607 -- __onexit (returns the registered function, or 0 when the table could not grow)
extern "C" uint32_t d2launch_onexit(LaunchOnexit* t, uint32_t func, const LaunchOnexitEnv& e) {
    e.lock8();
    uint32_t* before = t->next;
    d2launch_onexit_lk(t, func, e);
    // retail returns EAX left by __onexit_lk: func when stored, otherwise the failed state
    uint32_t r = (t->next != before) ? func : 0;
    e.unlock8();
    return r;
}

// 1.13c D2Launch+0x0363F -- _atexit
extern "C" int32_t d2launch_atexit(LaunchOnexit* t, uint32_t func, const LaunchOnexitEnv& e) {
    return (d2launch_onexit(t, func, e) != 0) - 1;
}
