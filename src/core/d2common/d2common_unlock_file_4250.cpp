// src/core/d2common/d2common_unlock_file_4250.cpp -- 1.13c D2Common.dll CRT __unlock_file.
#include <cstdint>

#include <Windows.h>   // LeaveCriticalSection: kernel state, a no-op under wasm

extern "C" void D2Common_2E44_Unlock(uint32_t nIdx);   // lock-table leaf, d2common_batch_tiny_1019i.cpp

// 1.13c D2Common+0x4250
// FILE in the static _iob table (retail VA 0x6FDE9308..0x6FDE9568, 32-byte entries, pFile held as the
// retail-layout token): release lock-table entry (slot + 0x10). Otherwise leave the FILE's own critical
// section at +0x20.
extern "C"
void D2Common_4250_UnlockFile(void* pFile)
{
    const uint32_t v = (uint32_t)(uintptr_t)pFile;
    if (v > 0x6FDE9307u && v < 0x6FDE9569u) {
        D2Common_2E44_Unlock(((v - 0x6FDE9308u) >> 5) + 0x10);
        return;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)((uint8_t*)pFile + 0x20));
}
