#include <cstdint>

// Prior art: none in D2MOO (linked MSVC CRT).
// 1.13c D2Common+0x028F6 (_flsall)
// Under lock 1, walks the CRT stream table (DAT_6fdf1820, DAT_6fdf2840 entries, handed in as
// pTable/nCount): for each stream whose _flag (+0xC) & 0x83 is set, lock it (0x422D), re-test, then
// mode 1 flushes all and counts successes, mode 0 flushes only writable (_flag & 2) streams and
// returns -1 if any flush failed. Unlock via the 0x2996 / 0x29C2 fragments.
extern "C" {
void D2Common_2E44_Unlock(uint32_t nIdx);
void D2Common_2996_UnlockFile2(void* pFile);
void D2Common_422D_LockFile2(int32_t nIndex, void* pFile);
int32_t D2Common_28C8_FFlushLk(void* stream);
void D2Common_29C2_Unlock1(void);
// ponytail: 0x2EF9 (__lock) is its own UNMAPPED row; seam supplied by the caller until then.
int32_t D2Common_28F6_FlsAll(int32_t mode, void** pTable, int32_t nCount, void (*pfnLock1)(void))
{
    int32_t nOk = 0, nErr = 0;
    pfnLock1();
    for (int32_t i = 0; i < nCount; ++i) {
        void* f = pTable[i];
        if (f && (*(uint8_t*)((uint8_t*)f + 0xC) & 0x83)) {
            D2Common_422D_LockFile2(i, f);
            f = pTable[i];
            uint32_t fl = *(uint32_t*)((uint8_t*)f + 0xC);
            if (fl & 0x83) {
                if (mode == 1) {
                    if (D2Common_28C8_FFlushLk(f) != -1) ++nOk;
                } else if (mode == 0 && (fl & 2)) {
                    if (D2Common_28C8_FFlushLk(f) == -1) nErr = -1;
                }
            }
            D2Common_2996_UnlockFile2(f);
        }
    }
    D2Common_29C2_Unlock1();
    return mode != 1 ? nErr : nOk;
}
}
