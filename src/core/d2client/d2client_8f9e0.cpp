// 1.13c D2Client+0x8F9E0 -- a unit's state/strength line (EAX = unit, [esp+4] = out wide string, ret 4). a, b, c = 0xD1EC(0xF9B, 0xF9E,
// 0xF9C); lvl = 0xC83E(self, unit); flag = 0xC98E(unit, 0x5D, 0) != 0. idx = 5 when lvl >= 28, 1 when lvl < 10, else
// T[(lvl*5 - 50) + K[(0xC1CC(unit,0x23) || 0xC1CC(unit,0x1B)) + 2 * (self ? self[+4] : -1)]] (T .data 0xE0F78, K .data 0xE10E0).
// The first of 15 6-byte entries (.data 0xE0EF0: dword stat, word string id) whose stat 0xC1CC(unit, stat) is set: out = str(id) a c a.
// Then a 256-char local = str(word .data 0xE0F50 + idx*6), coloured by 0xBF5F0(EDI = local, 3) when flag, and out += local, b.
#include "cl_env.h"
namespace {
inline uint32_t A(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
constexpr uint32_t G = 0x6FAB0000u;
void wcpy(volatile char16_t* d, const char16_t* s) { while ((*d = *s)) { d++; s++; } }
void wcat(volatile char16_t* d, const volatile char16_t* s) { while (*d) d++; while ((*d = *s)) { d++; s++; } }
const char16_t* S(const cl::Env& e, uint32_t id) { return (const char16_t*)(uintptr_t)e.H(0xd1ec, 0, 0, 0, 0, 0, 0, id); }
}
extern "C" void d2c_8f9e0(uint32_t unit, uint32_t out, const cl::Env& e) {
    const char16_t* a = S(e, 0xf9b);
    const char16_t* b = S(e, 0xf9e);
    const char16_t* c = S(e, 0xf9c);
    const int32_t lvl = (int32_t)e.H(0xc83e, e.R(G + 0x11bbfc), unit);
    const bool flag = e.H(0xc98e, unit, 0x5d, 0) != 0;
    uint32_t idx;
    if (lvl >= 0x1c) idx = 5;
    else if (lvl < 10) idx = 1;
    else {
        const uint32_t self = e.R(G + 0x11bbfc);
        const uint32_t t = (e.H(0xc1cc, unit, 0x23) || e.H(0xc1cc, unit, 0x1b)) ? 1u : 0u;
        const uint32_t cls = self ? e.R(self + 4) : 0xffffffffu;
        const uint32_t k = e.R(G + 0xe10e0 + (t + cls * 2) * 4);
        idx = e.R(G + 0xe0f78 + ((uint32_t)lvl * 5 - 0x32 + k) * 4);
    }
    volatile char16_t* o = (volatile char16_t*)(uintptr_t)out;
    for (uint32_t i = 0; i < 15; i++) {
        if (!e.H(0xc1cc, unit, e.R(G + 0xe0ef0 + i * 6))) continue;
        wcpy(o, S(e, e.R(G + 0xe0ef4 + i * 6) & 0xffff));
        wcat(o, a); wcat(o, c); wcat(o, a);
        break;
    }
    volatile char16_t buf[256];
    for (int i = 0; i < 256; i++) buf[i] = 0;
    wcpy(buf, S(e, e.R(G + 0xe0f50 + idx * 6) & 0xffff));
    if (flag) { e.H(0xbf5f0, 3); e.Edj(0xbf5f0, A(buf)); }
    wcat(o, buf);
    wcat(o, b);
}
