// src/core/d2common/d2common_batch_tiny_1019i.cpp -- five tiny 1.13c D2Common.dll rows from the
// cached decompiles in build-wasm/ghidra-d2common/decomp/0x<RVA>.c (alpha lane, stage A). Base
// 0x6FD50000; every RVA below is retail's. All five are MSVC 2003 Release CRT glue: tail fragments
// of stdio routines calling the two __unlock_file leaves (0x4250/0x427F, still UNMAPPED as separate
// rows), the caller of lock-table leaf 0x2E44, and that leaf itself.
#include <cstdint>

#include <Windows.h>   // LeaveCriticalSection: kernel state, a no-op under wasm

// The lock table behind 0x2E44 is the same DAT_6fde9048 (8-byte stride) modelled in
// d2common_crt_b24.cpp. Under the compat shim the critical section is a no-op, so the entry address
// is not observable; the index is the whole rule.
static uint8_t g_csTable[0x200];

// 1.13c D2Common+0x223E
// Eight-byte tail fragment (caller 0x21F5): __unlock_file(ESI) with the FILE* in the caller's live
// ESI, so it is handed in as a parameter. 0x4250 is a separate UNMAPPED row.
// ponytail: the release is a no-op until 0x4250 is implemented.
extern "C"
void D2Common_223E_UnlockFile(void* pFile)
{
    (void)pFile;
}

// 1.13c D2Common+0x2996
// Seventeen-byte tail fragment (caller 0x28F6): __unlock_file2(ESI, table[ESI]) where ESI is the
// lock index and the FILE* comes from DAT_6fdf1820 + index*4. Caller-frame operands, handed in as a
// parameter. 0x427F is a separate UNMAPPED row.
// ponytail: the release is a no-op until 0x427F is implemented.
extern "C"
void D2Common_2996_UnlockFile2(void* pFile)
{
    (void)pFile;
}

// 1.13c D2Common+0x2A11
// Ten-byte tail fragment (caller 0x29CB): __unlock_file(*(FILE**)[EBP+8]), the handle in the
// caller's frame slot. Same rule as 0x223E; 0x4250 is a separate UNMAPPED row.
// ponytail: the release is a no-op until 0x4250 is implemented.
extern "C"
void D2Common_2A11_UnlockFile(void* pFile)
{
    (void)pFile;
}

// 1.13c D2Common+0x2E44
// The CRT lock-table leaf: LeaveCriticalSection(&DAT_6fde9048 + index*8). 19 callers, all passing a
// lock index; host state behind the compat shim, as in d2common_crt_b24.cpp's 0x19BF.
extern "C"
void D2Common_2E44_Unlock(uint32_t nIdx)
{
    LeaveCriticalSection((LPCRITICAL_SECTION)(g_csTable + (size_t)nIdx * 8));
}

// 1.13c D2Common+0x29C2
// Nine-byte tail fragment (caller 0x28F6): release lock-table entry 1. The callee is our 0x2E44.
extern "C"
void D2Common_29C2_Unlock1(void)
{
    D2Common_2E44_Unlock(1);
}
