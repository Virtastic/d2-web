// 1.13c D2Client+0x8FB80 -- an item's stat-0x1F line (stdcall item, unit, out; ret 0xC). Nothing without an item. g = .data 0x11BC38;
// a, b = 0xD1EC(0xF9B, 0xF9E); alt = 0xC51A(g, &p) replaces the item when set; 0xC4FC(item, g, 1); v = 0xC93A(g); then 0xC544(item, g)
// without alt, else 0xC4FC(alt, g, p). Coloured when 0xC250(unit, 0x1F, 0) != v. Stats 0x10C / 0x10D of the unit (0xC1C6), when the
// table has the row (sgptDataTables +0xBD4 count, +0xBCC base + row*0x144 non-null) and item[+0x1C] is set, go through 0xC5FE(item[+0x1C], &q)
// and 0xC8C2(stat, r, q, 0, 0, 0): 0x10C adds its result, 0x10D adds result * v / 100 (0x14A0); either result non-zero colours the number.
// out = str(0xD85) a, then "%i" of v (Storm 578 into 10 bytes, win2Unicode 9 chars), coloured by 0xBF5F0(EDI = number, 3), then b.
#include "cl_env.h"
namespace {
inline uint32_t A(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
constexpr uint32_t G = 0x6FAB0000u;
void wcpy(volatile char16_t* d, const char16_t* s) { while ((*d = *s)) { d++; s++; } }
void wcat(volatile char16_t* d, const volatile char16_t* s) { while (*d) d++; while ((*d = *s)) { d++; s++; } }
const char16_t* S(const cl::Env& e, uint32_t id) { return (const char16_t*)(uintptr_t)e.H(0xd1ec, 0, 0, 0, 0, 0, 0, id); }
// 1.13c D2Client+0x14A0: a * b / d (fastcall ECX = d, EDX = b, EAX = a), 0 when d is 0, dividing first when a 32-bit product could overflow
int32_t mulDiv(int32_t a, int32_t b, int32_t d) {
    if (!d) return 0;
    if (b > 0x100000) { if (d > (b >> 4)) return (int32_t)((int64_t)b * a / d); return (int32_t)((uint32_t)(b / d) * (uint32_t)a); }
    if (a <= 0x10000) return (int32_t)(uint32_t)((uint32_t)b * (uint32_t)a) / d;
    if (d > (a >> 4)) return (int32_t)((int64_t)b * a / d);
    return (int32_t)((uint32_t)(a / d) * (uint32_t)b);
}
// Storm 578 (10-byte buffer, "%i") then win2Unicode (9 chars): the decimal text of v, at most 9 characters
void num(volatile char16_t* w, int32_t v) {
    char t[12]; int k = 0; uint32_t m = v < 0 ? 0u - (uint32_t)v : (uint32_t)v;
    do { t[k++] = (char)('0' + m % 10); m /= 10; } while (m);
    if (v < 0) t[k++] = '-';
    int n = 0; while (k && n < 9) w[n++] = (char16_t)t[--k];
    w[n] = 0;
}
}
extern "C" void d2c_8fb80(uint32_t item, uint32_t unit, uint32_t out, const cl::Env& e) {
    if (!item) return;
    volatile char16_t wide[10];
    for (int i = 0; i < 10; i++) wide[i] = 0;
    const char16_t* a = S(e, 0xf9b);
    const char16_t* b = S(e, 0xf9e);
    volatile uint32_t p = 0;
    const uint32_t alt = e.H(0xc51a, e.R(G + 0x11bc38), A(&p));
    if (alt && item != alt) item = alt;
    e.H(0xc4fc, item, e.R(G + 0x11bc38), 1);
    int32_t v = (int32_t)e.H(0xc93a, e.R(G + 0x11bc38));
    if (!alt) e.H(0xc544, item, e.R(G + 0x11bc38));
    else e.H(0xc4fc, alt, e.R(G + 0x11bc38), p);
    bool col = (int32_t)e.H(0xc250, unit, 0x1f, 0) != v;
    for (uint32_t k = 0; k < 2; k++) {
        const uint32_t stat = 0x10c + k;
        const uint32_t s = e.H(0xc1c6, unit, stat, 0);
        if (!s || (int32_t)e.R(e.dt + 0xbd4) <= (int32_t)stat || e.R(e.dt + 0xbcc) + stat * 0x144 == 0) continue;
        const uint32_t x = e.R(item + 0x1c);
        if (!x) continue;
        volatile uint32_t q = 0;
        const uint32_t r = e.H(0xc5fe, x, A(&q));
        const int32_t t = (int32_t)e.H(0xc8c2, s, r, q, 0, 0);
        v += k ? mulDiv(t, v, 100) : t;
        if (t) col = true;
    }
    volatile char16_t* o = (volatile char16_t*)(uintptr_t)out;
    wcpy(o, S(e, 0xd85));
    wcat(o, a);
    num(wide, v);
    if (col) { e.H(0xbf5f0, 3); e.Edj(0xbf5f0, A(wide)); }
    wcat(o, wide);
    wcat(o, b);
}
