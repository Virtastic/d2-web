// 1.13c D2Client+0x1F9B0: resistance cap pass after a stat change. fastcall(ECX = unit, EDX = type, [esp+4] = flag), ret 4.
// Reads six stats of the unit through the D2Common stat getter (thunk 0xC1C6, stdcall unit, stat, layer 0) and raises them
// through the setter (thunk 0xC40C, stdcall unit, stat, value, layer 0). The count c of stats at 100 or more gates every raise.
#include "d2client_callee.h"
namespace {
constexpr uint32_t kGet = 0xc1c6, kSet = 0xc40c;
}
// 1.13c D2Client+0x1F9B0
extern "C" uint32_t d2c_1f9b0(const D2cSvc& v, uint32_t unit, uint32_t type, uint32_t flag) {
    uint32_t eax = 0;                                      // retail returns the flag (0) when it is clear, else the last EAX
    if (!flag) return 0;
    const auto get = [&](uint32_t id) { return eax = d2c_call(v, kGet, 0, 0, 0, 0, 0, unit, id, 0); };
    const auto set = [&](uint32_t id, int32_t val) { eax = d2c_call(v, kSet, 0, 0, 0, 0, 0, unit, id, (uint32_t)val, 0); };
    if (type == 0x1c) { int32_t t = (int32_t)get(0x1f); set(0x1f, t << 1); }
    int32_t a = (int32_t)get(0x27), b = (int32_t)get(0x29), c27 = (int32_t)get(0x2b), d = (int32_t)get(0x2d), e = (int32_t)get(0x24), f = (int32_t)get(0x25);
    int32_t n = 0;                                         // c: starts at 0, the first test stores 1 (same value)
    if (a >= 100) n = 1;
    if (b >= 100) n++;
    if (c27 >= 100) n++;
    if (d >= 100) n++;
    if (e >= 100) n++;
    if (f >= 100) n++;
    if (n >= 2) return eax;
    switch (type) {
    case 8:
        if (c27 < 100) { c27 += 0x28; set(0x2b, c27); if (c27 >= 100) n++; }
        if (n >= 2) return eax;
        if (a < 100) { a += 0x28; set(0x27, a); if (a >= 100) n++; }
        if (n >= 2) return eax;
        if (b >= 100) return eax;
        b += 0x28; set(0x29, b);
        if (b >= 100) return (uint32_t)(n + 1);
        return eax;
    case 9: set(0x27, a + 0x4b); return eax;
    case 0x12: set(0x2b, c27 + 0x4b); return eax;
    case 0x11: set(0x29, b + 0x4b); return eax;
    case 0x17: set(0x2d, d + 0x4b); return eax;
    case 0x19: set(0x25, f + 0x14); return eax;
    case 0x1b:
        if (c27 < 0x4b) { c27 += 0x14; set(0x2b, c27); if (c27 >= 100) n++; }
        if (n >= 2) return eax;
        if (a < 0x4b) { a += 0x14; set(0x27, a); if (a >= 100) n++; }
        if (n >= 2) return eax;
        if (b >= 0x4b) return eax;
        b += 0x14; set(0x29, b);
        if (b >= 100) return (uint32_t)(n + 1);
        return eax;
    case 0x1c: set(0x24, e + 0x32); return eax;
    default: return eax;
    }
}
