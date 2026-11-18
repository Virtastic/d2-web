// 1.13c D2Client+0x8D880 -- a unit's stat-0x46 line (ESI = out string, [esp+4] = unit): a 10-char local zeroed, prefix a = 0xD1EC(0xF9B),
// suffix b = 0xD1EC(0xF9E). Nothing when 0xC1C6(unit, 0x46, 0) <= 0 and 0xC91C(unit) <= 0. Otherwise out = 0xD1EC(0xD86); the stat
// printed "%ld" through Storm 578 into a 10-byte local, widened (win2Unicode, 9 chars) into the 10-char local, and out += a, number, b.
#include "cl_env.h"
namespace {
inline uint32_t A(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
void wcat(char16_t* d, const volatile char16_t* s) { while (*d) d++; while ((*d = *s)) { d++; s++; } }
}
extern "C" void d2c_8d880(uint32_t out, uint32_t unit, const cl::Env& e) {
    volatile char16_t num[10];
    for (int i = 0; i < 10; i++) num[i] = 0;
    const char16_t* a = (const char16_t*)(uintptr_t)e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xf9b);
    const char16_t* b = (const char16_t*)(uintptr_t)e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xf9e);
    if ((int32_t)e.H(0xc1c6, unit, 0x46, 0) <= 0 && (int32_t)e.H(0xc91c, unit) <= 0) return;
    char16_t* o = (char16_t*)(uintptr_t)out;
    const char16_t* t = (const char16_t*)(uintptr_t)e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xd86);
    while ((*o = *t)) { o++; t++; }
    o = (char16_t*)(uintptr_t)out;
    uint32_t v = e.H(0xc1c6, unit, 0x46, 0);
    volatile char tmp[12];
    e.H(0xbea2, A(tmp), 10, 0xB20D35A0u, v);
    e.H(0xceac0, 9, 0, 0, 0, 0, 0, A(num), A(tmp));
    wcat(o, a); wcat(o, num); wcat(o, b);
}
