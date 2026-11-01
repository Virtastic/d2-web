// Register-arg helper that adds the weapon/elemental pair of the two helpers, scaled by esi/128, into two caller words.
#include "d2client_33240.h"
namespace d2client_33240 {
static int32_t div128(int32_t x) { return (x + ((x >> 31) & 0x7f)) >> 7; }  // signed x/128 as the compiler emits it (cdq; and 0x7f; add; sar 7)
// esi == 0: nothing (eax = 0). Otherwise two zeroed locals mn/mx; the first helper runs with (ecx = unit, eax = 0; stack &mn, &mx, E, 0, edi, 0x80,
// ecx, 0) with E = the incoming eax and ecx the incoming ecx; the second helper runs with (unit, &mn, &mx, E, edi); *pX += mn*esi/128; *pY += mx*esi/128.
// The result is the last quotient (mx*esi/128).
uint32_t sub_34D70(Ops& o, uint32_t eax, uint32_t ecx, int32_t esi, uint32_t edi, uint32_t unit, int32_t* pX, int32_t* pY) {
    if (!esi) return 0;
    int32_t mn = 0, mx = 0;
    const uint32_t w[8] = {kLocal, kLocal, eax, 0, edi, 0x80, ecx, 0};
    o.c31220(unit, 0, w);
    o.c30E30loc(unit, &mn, &mx, eax, edi);
    *pX += div128(int32_t(uint32_t(mn) * uint32_t(esi)));
    const int32_t q = div128(int32_t(uint32_t(mx) * uint32_t(esi)));
    *pY += q;
    return uint32_t(q);
}
}
