// src/core/d2common/d2common_mtinitlocks_2da6.cpp -- 1.13c D2Common.dll __mtinitlocks (VC7.1 CRT).
#include <cstdint>

struct D2CommonMtLocksEnv {
    int32_t (*init_cs_spin)(void* cs, uint32_t spin);  // ___crtInitCritSecAndSpinCount (0x5E4D)
};

// 1.13c D2Common+0x02DA6 (73 B): table DAT_6fde9048, 0x24 records of 8 bytes {cs ptr, preallocate flag}.
// Each record whose flag (+4) is 1 gets the next 0x18-byte slice of the static pool DAT_6fdef600 as its
// critical section and it is initialised with spin 4000; on failure that record's pointer is cleared and
// 0 returned. Returns 1 when all are set. Only host state is touched (no observable bytes).
extern "C" int32_t D2Common_2DA6_MtInitLocks(uint32_t (*tbl)[2], uint8_t* pool, const D2CommonMtLocksEnv* env)
{
    for (int i = 0; i < 0x24; i++) {
        if (tbl[i][1] == 1) {
            tbl[i][0] = (uint32_t)(uintptr_t)pool;
            pool += 0x18;
            if (env->init_cs_spin(pool - 0x18, 4000) == 0) { tbl[i][0] = 0; return 0; }
        }
    }
    return 1;
}
