// 1.13c D2Common+0x71180:item-types unload. The equivalence-LUT pool (0xA0ADC) is freed
// unconditionally (ItemTbls.cpp line 0x8A7, file 0), then a set item-types array (0xA0AD0) as a .bin block (DataTbls.cpp,
// file 1: block less 4 at 0x8F6 when the .bin flag is set, else the block at 0x8F8), then the linker (0xA0ACC) is released.
// The two frees are the caller's seams.
#include <cstdint>

struct D2Common71180State { uintptr_t pLinker, pItemTypesTxt, pEquivalenceLUTs; uint32_t bFromBin; };

extern "C" void D2Common_71180_Unload(D2Common71180State* s, void (*pfFree)(uintptr_t, int32_t, int32_t),
                                      void (*pfFreeLinker)(uintptr_t))
{
    pfFree(s->pEquivalenceLUTs, 0, 0x8A7);
    if (s->pItemTypesTxt != 0)
    {
        if (s->bFromBin) pfFree(s->pItemTypesTxt - 4, 1, 0x8F6);
        else pfFree(s->pItemTypesTxt, 1, 0x8F8);
    }
    pfFreeLinker(s->pLinker);
}
