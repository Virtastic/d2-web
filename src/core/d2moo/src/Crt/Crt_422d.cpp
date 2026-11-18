// 1.13c D2Common+0x422D __lock_file2(index, file). D2MOO has no twin (host CRT).
#include <cstdint>
#include <Windows.h>   // EnterCriticalSection: kernel state, a no-op under wasm

// 1.13c D2Common+0x422D
// Stream slots below 0x14 live in the static lock table, so take table lock index+0x10 through pfnLock (the 0x2EF9 __lock
// of the caller's table); otherwise the FILE carries its own critical section at +0x20.
void d2common_lock_file2_422d(int32_t nIndex, void* pFile, void (*pfnLock)(int32_t))
{
    if (nIndex < 0x14) pfnLock(nIndex + 0x10);
    else EnterCriticalSection((LPCRITICAL_SECTION)((uint8_t*)pFile + 0x20));
}
