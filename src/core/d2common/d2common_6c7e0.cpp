// 1.13c D2Common+0x6C7E0:levels-table unload. A set record array (sgptDataTables+0xC58) is
// released through the Fog free (DataTbls.cpp): the block less 4 at line 0x8F6 when the module's .bin flag is set, else the
// block at line 0x8F8. The record count (+0xC5C) is zeroed either way. The free is the caller's seam.
#include <cstdint>

struct D2Common6C7E0State { uintptr_t pLevelsTxt; int32_t nLevelsTxtRecordCount; uint32_t bFromBin; };

extern "C" void D2Common_6C7E0_Unload(D2Common6C7E0State* s, void (*pfFree)(uintptr_t, int32_t))
{
    if (s->pLevelsTxt != 0)
    {
        if (s->bFromBin) pfFree(s->pLevelsTxt - 4, 0x8F6);
        else pfFree(s->pLevelsTxt, 0x8F8);
    }
    s->nLevelsTxtRecordCount = 0;
}
