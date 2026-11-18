// 1.13c D2Common+0x3445 __mtterm: free the FLS/TLS slot if allocated, then delete the CRT locks. the 1.10f source has no twin (host CRT).
#include <cstdint>

struct D2CommonMtTermEnv { int32_t* flsIndex; void (*flsFree)(int32_t); void (*deleteLocks)(); };

// 1.13c D2Common+0x3445
void d2common_mtterm_3445(const D2CommonMtTermEnv& e)
{
    if (*e.flsIndex != -1) { e.flsFree(*e.flsIndex); *e.flsIndex = -1; }
    e.deleteLocks();
}
