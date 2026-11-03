// 1.13c D2Client+0x52F10: the screen-fade / pulse step. No arguments; the state is the 11-dword block at
// 0x6FBC960C and the unit id at 0x6FBCBBFC. Callees come in through Env: 0xA9A20 (light colour: a dword and three bytes), the unit lookup
// (Ordinal 10331) and its result's id (10826); the cosine is Fog 10083 (fog_sub_16730) and the multiply is fild(uint32 local) * cos, _ftol.
#include <cstdint>
#include <cstring>
#include "d2client_52f10.h"
namespace d2client_52f10 {
// float bits * unsigned local, truncated toward zero (x87 fild/fmul/_ftol; the product is exact in extended precision)
static int32_t scale(uint32_t local, uint32_t fbits) {
    uint32_t e = fbits >> 23 & 0xFF; if (e == 0) return 0;
    uint64_t m = (fbits & 0x7FFFFF) | 0x800000, p = (uint64_t)local * m;
    int sh = 150 - (int)e; uint64_t v = sh >= 64 ? 0 : sh >= 0 ? p >> sh : p << -sh;
    return (int32_t)(fbits >> 31 ? (uint32_t)(0 - v) : (uint32_t)v);
}
// 1.13c D2Client+0x52F10
void progress(State& s, int32_t cbbfc, Env& env) {
    if (cbbfc) env.unit(cbbfc);
    uint32_t local = env.f59a20(&s.c9614, &s.c9615, &s.c9616);
    int32_t n = s.c9628;
    if (n < s.c9618) {
        s.c9610 = scale(local, (uint32_t)env.cosine(s.c962c));
        s.c962c = (int32_t)(((int64_t)(n << 7)) / s.c9618);   // 32-bit shl, cdq, idiv
    } else if (n >= s.c9624 - s.c961c) {
        s.c9610 = scale(local, (uint32_t)env.cosine(s.c962c));
        int32_t ecx = s.c961c, eax = ecx - s.c9624 + s.c9628;
        if (ecx == 0) s.c961c = ecx = 0x19;
        s.c962c = 0x80 - (eax << 7) / ecx;
    } else s.c9610 = 0;
    s.c9628 = n + 1;
    int32_t v = cbbfc && env.unit(cbbfc) ? env.lookup(cbbfc) : 0;
    if (s.c9628 > s.c9624 || v != s.c9630) memset(&s, 0, 44);
}
}  // namespace d2client_52f10
