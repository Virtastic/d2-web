// D2Game+0xDE8F0 -- split one amount over the units a context walk collects. EAX = unit, stack a0 (context), a1, a2 (value), a3, a4 (base amount), ret 0x14.
// Written from the 1.13c disassembly. 0x9C070(EAX = unit, EBX = a0; callback 0xFAED0, &L) fills a 19-word record: L[0] = a1 on entry, L[1..8] = unit
// list, L[9..16] = the matching values, L[17] = count n, L[18] = divisor m. n or m <= 0 does nothing. n == 1: 0xDC030(EAX = a4, EBX = unit; a0, a3) then
// 0xDE6D0(EAX = unit; a0, a2, result). n > 1: t = a4 + (((n-1) * a4 * 0x59 + (neg ? 0xFF : 0)) >> 8); f = (float)(t / m) (x87 fild/fidiv/fstp m32);
// per unit i: 0xDC030(EAX = trunc(L[9+i] * f), EBX = L[1+i]; a0, a3), 0xDE6D0(EAX = L[1+i]; a0, L[9+i], result). The Pentium fdiv-bug path is not modelled.
#include "d2game_seam4e.h"
// 1.13c D2Game+0xDE8F0
extern "C" void d2game_de8f0(uint32_t unit, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, const D2Seam* e)
{
    uint32_t L[19] = {};
    L[0] = a1;
    e->call(0x9C070, unit, 0, 0, a0, 0, 0xB2000000u | 0xFAED0u, (uint32_t)(uintptr_t)L);
    const int32_t n = (int32_t)L[17], m = (int32_t)L[18];
    if (n <= 0 || m <= 0) return;
    if (n == 1) {
        const uint32_t r = e->call(0xDC030, a4, 0, 0, unit, 0, a0, a3);
        e->call(0xDE6D0, unit, 0, 0, 0, 0, a0, a2, r);
        return;
    }
    const int32_t p = (int32_t)((uint32_t)(n - 1) * a4 * 0x59u);
    const int32_t t = (int32_t)((uint32_t)(((p + (int32_t)((uint32_t)(p >> 31) & 0xFFu)) >> 8)) + a4);
    const float f = (float)((long double)t / (long double)m);
    for (int32_t i = 0; i < n; i++) {
        const uint32_t edi = L[9 + i], ebx = L[1 + i];
        const long double q = (long double)(int32_t)edi * (long double)f;
        const uint32_t v = (q >= 2147483648.0L || q <= -2147483649.0L) ? 0x80000000u : (uint32_t)(int32_t)q;   // _ftol: out of range = 0x80000000
        const uint32_t r = e->call(0xDC030, v, 0, 0, ebx, 0, a0, a3);
        e->call(0xDE6D0, ebx, 0, 0, 0, 0, a0, edi, r);
    }
}
