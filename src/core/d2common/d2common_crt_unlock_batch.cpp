// Prior art used: existing D2Common CRT lock owner (0x2E44) and Windows.h
// compatibility boundary. D2MOO's LevelsTbls.cpp confirms that boundary, but
// does not contain these MSVC CRT fragments; indices are from cached 1.13c RE.
#include "d2common_crt_unlock_batch.h"
#include "d2common_crt.h"

extern "C" void D2Common_2E44_Unlock(uint32_t index);

// 1.13c D2Common+0x62A1
extern "C" void D2Common_62A1_Unlock4()
{
    D2Common_2E44_Unlock(4);
}

// 1.13c D2Common+0x722E
extern "C" void D2Common_722E_Unlock13()
{
    D2Common_2E44_Unlock(13);
}

// 1.13c D2Common+0x7B32
extern "C" void D2Common_7B32_Unlock10()
{
    d2common_unlock_7b32();
}

// 1.13c D2Common+0x7C36
extern "C" void D2Common_7C36_Unlock10()
{
    D2Common_2E44_Unlock(10);
}

// 1.13c D2Common+0x7CD0
extern "C" void D2Common_7CD0_Unlock11()
{
    D2Common_2E44_Unlock(11);
}

// 1.13c D2Common+0x5E3D
// The CRT no-spin fallback initializes the supplied section, then returns TRUE.
// 0x5E4D stores this in the two-argument initializer slot (retail stdcall @8);
// the spin count is ignored, but remains part of the callable signature.
extern "C" int32_t D2Common_5E3D_InitCriticalSection(LPCRITICAL_SECTION section, uint32_t /*spinCount*/)
{
    InitializeCriticalSection(section);
    return 1;
}
