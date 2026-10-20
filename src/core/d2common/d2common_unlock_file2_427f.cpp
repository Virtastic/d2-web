// src/core/d2common/d2common_unlock_file2_427f.cpp -- 1.13c D2Common.dll CRT __unlock_file2.
#include <cstdint>

#include <Windows.h>   // LeaveCriticalSection: kernel state, a no-op under wasm

extern "C" void D2Common_2E44_Unlock(uint32_t nIdx);   // lock-table leaf, d2common_batch_tiny_1019i.cpp

// 1.13c D2Common+0x0427f
// Index below 20: release lock-table entry (index + 0x10). Otherwise the FILE carries its own
// critical section at +0x20; leave it.
extern "C"
void D2Common_427F_UnlockFile2(int32_t nIndex, void* pFile)
{
    if (nIndex < 0x14) {
        D2Common_2E44_Unlock((uint32_t)(nIndex + 0x10));
        return;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)((uint8_t*)pFile + 0x20));
}
