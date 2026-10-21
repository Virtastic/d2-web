// src/core/d2common/d2common_mtdeletelocks_2def.cpp -- 1.13c D2Common.dll __mtdeletelocks (VC7.1 CRT).
// Base 0x6FD50000. DeleteCriticalSection and _free are host seams passed in (mtinitlocknum pattern).
#include <cstdint>

struct D2CommonMtDelEnv {
    void (*delete_cs)(void* cs);   // DeleteCriticalSection
    void (*free_)(void* p);        // _free (0x4C0D)
};

// 1.13c D2Common+0x2DEF (85 B): the lock table (DAT_6fde9048..0x6fde9168, 36 entries of {cs*, flag},
// 8-byte stride). Pass 1: every non-null entry whose flag != 1 is deleted, freed and nulled. Pass 2:
// every non-null entry whose flag == 1 (statically owned) is only deleted, not freed or nulled.
extern "C" void D2Common_2DEF_MtDeleteLocks(uint32_t (*table)[2], const D2CommonMtDelEnv* env)
{
    for (int i = 0; i < 36; i++)
        if (table[i][0] && table[i][1] != 1) {
            void* cs = (void*)(uintptr_t)table[i][0];
            env->delete_cs(cs);
            env->free_(cs);
            table[i][0] = 0;
        }
    for (int i = 0; i < 36; i++)
        if (table[i][0] && table[i][1] == 1)
            env->delete_cs((void*)(uintptr_t)table[i][0]);
}
