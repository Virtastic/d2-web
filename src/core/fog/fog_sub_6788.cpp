// Fog+0x6788: nine-byte CRT lock leaf, _unlock(4) (LeaveCriticalSection on lock-table entry 4,
// Fog+0x3C3F). Lock table is host state; no-op behind the Windows compat shim. Used by __msize.
#include <cstdint>

#include <Windows.h>

static uint8_t g_fog_csTable6788[0x400];

extern "C" void fog_sub_6788(void) {  // 1.13c Fog+0x06788
    LeaveCriticalSection((LPCRITICAL_SECTION)(g_fog_csTable6788 + (size_t)4 * 8));
}
