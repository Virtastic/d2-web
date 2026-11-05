// 1.13c D2Client+0x90770 -- an item's quantity line (ESI = out; stdcall item; ret 4). a, b = 0xD1EC(0xF9B, 0xF9E). Nothing unless
// 0xC8BC(item), 0xC916(item) > 0 and 0xC76C(item) == 0. Coloured when 0xC98E(item, 0x4B, 0) != 0. out = str(0xD81) a, "%i" of
// 0xC1C6(item, 0x48, 0), a, str(0xD87) a, then "%i" of 0xC916(item) (coloured by 0xBF5F0(EDI = number, 3)), b. The numbers go through
// Storm 578 (10 bytes) and win2Unicode (9 chars) into one 10-char local.
#include "cl_env.h"
namespace {
inline uint32_t A(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
void wcpy(volatile char16_t* d, const char16_t* s) { while ((*d = *s)) { d++; s++; } }
void wcat(volatile char16_t* d, const volatile char16_t* s) { while (*d) d++; while ((*d = *s)) { d++; s++; } }
const char16_t* S(const cl::Env& e, uint32_t id) { return (const char16_t*)(uintptr_t)e.H(0xd1ec, 0, 0, 0, 0, 0, 0, id); }
void num(volatile char16_t* w, int32_t v) {
    char t[12]; int k = 0; uint32_t m = v < 0 ? 0u - (uint32_t)v : (uint32_t)v;
    do { t[k++] = (char)('0' + m % 10); m /= 10; } while (m);
    if (v < 0) t[k++] = '-';
    int n = 0; while (k && n < 9) w[n++] = (char16_t)t[--k];
    w[n] = 0;
}
}
extern "C" void d2c_90770(uint32_t out, uint32_t item, const cl::Env& e) {
    volatile char16_t wide[10];
    for (int i = 0; i < 10; i++) wide[i] = 0;
    const char16_t* a = S(e, 0xf9b);
    const char16_t* b = S(e, 0xf9e);
    if (!e.H(0xc8bc, item) || (int32_t)e.H(0xc916, item) <= 0 || e.H(0xc76c, item)) return;
    const bool col = e.H(0xc98e, item, 0x4b, 0) != 0;
    volatile char16_t* o = (volatile char16_t*)(uintptr_t)out;
    wcpy(o, S(e, 0xd81));
    const int32_t q = (int32_t)e.H(0xc1c6, item, 0x48, 0);
    wcat(o, a);
    num(wide, q);
    wcat(o, wide);
    wcat(o, a);
    wcat(o, S(e, 0xd87));
    wcat(o, a);
    num(wide, (int32_t)e.H(0xc916, item));
    if (col) { e.H(0xbf5f0, 3); e.Edj(0xbf5f0, A(wide)); }
    wcat(o, wide);
    wcat(o, b);
}
