// src/core/d2common/d2common_mtinitlocknum_2e59.cpp -- 1.13c D2Common.dll __mtinitlocknum (VC7.1 CRT).
// Base 0x6FD50000. The allocator, errno slot, __lock/_unlock(10) and the critical-section init are
// other rows / host seams, passed in (src/core/realm/mcp_crt_lock_init.cpp pattern).
#include <cstdint>

struct D2CommonMtLockEnv {
    void*   (*malloc_)(uint32_t n);                        // 0x4C0D-family _malloc
    void    (*free_)(void* p);                             // _free
    int32_t* (*errno_ptr)();                               // 0x44BC
    void    (*lock10)();                                   // __lock(10)  (0x2EF9)
    void    (*unlock10)();                                 // 0x2EF0 _unlock(10)
    int32_t (*init_cs_spin)(void* cs, uint32_t spin);      // ___crtInitCritSecAndSpinCount
};

// 1.13c D2Common+0x2E59 (151 B): make sure lock-table slot `slot` (DAT_6fde9048, 8-byte stride; the
// dword at the slot is the critical-section pointer) holds an allocated 0x18-byte critical section.
// Allocate first; on failure set errno=12 and return 0. Under lock 10 re-check the slot (another thread
// may have filled it): if still empty init the section with spin 4000 (failure: free, errno=12, unlock
// via the SEH unwind, return 0) and store it; otherwise free the spare. Unlock 10, return 1.
extern "C" int32_t D2Common_2E59_MtInitLockNum(uint32_t* slot, const D2CommonMtLockEnv* env)
{
    if (*slot != 0) return 1;
    void* cs = env->malloc_(0x18);
    if (!cs) { *env->errno_ptr() = 12; return 0; }
    env->lock10();
    if (*slot == 0) {
        if (env->init_cs_spin(cs, 4000) == 0) {
            env->free_(cs);
            *env->errno_ptr() = 12;
            env->unlock10();   // __local_unwind2 runs the __finally -> _unlock(10)
            return 0;
        }
        *slot = (uint32_t)(uintptr_t)cs;
    } else {
        env->free_(cs);
    }
    env->unlock10();
    return 1;
}
