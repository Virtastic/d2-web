// 1.13c D2Client+0x8FDC0 -- an item's percent line (EAX = unit; stdcall out, rec; ret 8). v = 0xC1C6(unit, 0x14, 0); with a player
// (self .data 0x11BBFC): v += byte +0x49 of its class row (sgptDataTables +0xBC4 base, +0xBC8 count, stride 0xC4), and when
// sk = 0xC838(ECX self, EDX 0x75) is set and 0xC1FC(self, 0x65): v += 0xC86E(0xC718(self, sk, 1), 0x75); v is capped at 75 (a player only).
// Nothing when v is 0. Else the number "%d%%\n" (Storm 578, 32 bytes; win2Unicode 32 chars) is coloured by 0xBF5F0(EDI = number, 3) when
// v > byte rec[+0x111]; out = str(0x2B0A) through a 256-char local coloured by 0xBF5F0(EDI = local, 0), then out += number.
#include "cl_env.h"
namespace {
inline uint32_t A(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
constexpr uint32_t G = 0x6FAB0000u;
void wcpy(volatile char16_t* d, const volatile char16_t* s) { while ((*d = *s)) { d++; s++; } }
void wcat(volatile char16_t* d, const volatile char16_t* s) { while (*d) d++; while ((*d = *s)) { d++; s++; } }
}
extern "C" void d2c_8fdc0(uint32_t unit, uint32_t out, uint32_t rec, const cl::Env& e) {
    int32_t v = (int32_t)e.H(0xc1c6, unit, 0x14, 0);
    const uint32_t self = e.R(G + 0x11bbfc);
    if (self) {
        const int32_t cls = (int32_t)e.R(self + 4);
        const uint32_t row = cls >= 0 && cls < (int32_t)e.R(e.dt + 0xbc8) ? e.R(e.dt + 0xbc4) + (uint32_t)cls * 0xc4 : 0;
        v += (int32_t)e.R8(row + 0x49);
        const uint32_t sk = e.H(0xc838, 0, 0, 0, 0, 0, 0, self, 0x75);
        if (sk && e.H(0xc1fc, self, 0x65)) v += (int32_t)e.H(0xc86e, e.H(0xc718, self, sk, 1), 0x75);
        if (v > 0x4b) v = 0x4b;
    }
    if (!v) return;
    volatile char16_t num[0x20], big[0x100];
    for (int i = 0; i < 0x20; i++) num[i] = 0;
    for (int i = 0; i < 0x100; i++) big[i] = 0;
    {   // "%d%%\n"
        char t[12]; int k = 0, n = 0; uint32_t m = v < 0 ? 0u - (uint32_t)v : (uint32_t)v;
        do { t[k++] = (char)('0' + m % 10); m /= 10; } while (m);
        if (v < 0) num[n++] = '-';
        while (k) num[n++] = (char16_t)t[--k];
        num[n++] = '%'; num[n++] = '\n'; num[n] = 0;
    }
    if (v > (int32_t)e.R8(rec + 0x111)) { e.H(0xbf5f0, 3); e.Edj(0xbf5f0, A(num)); }
    wcpy(big, (const char16_t*)(uintptr_t)e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x2b0a));
    e.H(0xbf5f0, 0); e.Edj(0xbf5f0, A(big));
    volatile char16_t* o = (volatile char16_t*)(uintptr_t)out;
    wcpy(o, big);
    wcat(o, num);
}
