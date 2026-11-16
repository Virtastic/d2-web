// 1.13c D2Common+0x716B0:sets-tables unload. Linker A (0xA0AE0) then linker B (0xA0AEC) are
// released; then slot A (0xA0AF0) and slot B (0xA0AE4), each when set, as .bin blocks through the Fog free (DataTbls.cpp: block
// less 4 at 0x8F6 when the .bin flag is set, else the block at 0x8F8); both counts (0xA0AE8, 0xA0AF4) are zeroed.
#include <cstdint>

struct D2Common716B0State { uintptr_t pLinkerA, pSlotB; uint32_t nCountB; uintptr_t pLinkerB, pSlotA; uint32_t nCountA; uint32_t bFromBin; };

static void FreeBin(uintptr_t p, uint32_t bFromBin, void (*pfFree)(uintptr_t, int32_t))
{
    if (p == 0) return;
    if (bFromBin) pfFree(p - 4, 0x8F6);
    else pfFree(p, 0x8F8);
}

extern "C" void D2Common_716B0_Unload(D2Common716B0State* s, void (*pfFree)(uintptr_t, int32_t), void (*pfFreeLinker)(uintptr_t))
{
    pfFreeLinker(s->pLinkerA);
    pfFreeLinker(s->pLinkerB);
    FreeBin(s->pSlotA, s->bFromBin, pfFree);
    FreeBin(s->pSlotB, s->bFromBin, pfFree);
    s->nCountB = 0;
    s->nCountA = 0;
}
