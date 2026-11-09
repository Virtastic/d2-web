// 1.13c D2Client+0xBFA10 -- fill the frame defaults of the eight skill-bar slots. EDI = s: eight slot indices s[0..7]
// (-1 = unused) and s[8] = the pointer to 12-byte entries whose +4 dword takes the frame.
// Per slot, in the order s[1], s[0], s[4], (#10227 non-zero) s[6], s[3], s[2], s[5], (#10227 non-zero) s[7]: if the index is not -1,
// 0x398C0 (EDX = key, ESI = sel; flag test) and then 0x38E50 (ESI = key, EDX = sel; frame getter, AX) are asked; a flag set, AX != 0xFFFF
// and AX <= 0xDF store AX zero-extended, anything else stores the default. keys/defaults: 0x16/0xC0, 0x26/0x1B, 0x29/0x4E, 0x2C/0x57;
// sel is 1 for the first four slots, 0 for the second four. The 0xBF8C thunk (Ordinal 10227) is called before each 0x2C slot.
#include <cstdint>
#include "cl_env.h"
extern "C" void d2c_bfa10(const cl::Env& e, uint32_t s) {
    struct S { int slot; uint32_t key, sel, def; int gate; };
    static const S t[] = { {1, 0x16, 1, 0xc0, 0}, {0, 0x26, 1, 0x1b, 0}, {4, 0x29, 1, 0x4e, 0}, {6, 0x2c, 1, 0x57, 1},
                           {3, 0x16, 0, 0xc0, 0}, {2, 0x26, 0, 0x1b, 0}, {5, 0x29, 0, 0x4e, 0}, {7, 0x2c, 0, 0x57, 1} };
    for (const S& x : t) {
        if (x.gate && !e.H(0xbf8c)) continue;
        int32_t i = (int32_t)e.R(s + x.slot * 4);
        if (i == -1) continue;
        uint32_t dst = e.R(s + 0x20) + (uint32_t)i * 12 + 4;
        if (e.H(0x398c0, 0, 0, 0, 0, 0, 0, 0, x.key, 0, x.sel)) {
            uint32_t ax = e.H(0x38e50, 0, 0, 0, 0, 0, 0, 0, x.sel, 0, x.key) & 0xffff;
            if (ax != 0xffff && ax <= 0xdf) { e.W(dst, ax); continue; }
        }
        e.W(dst, x.def);
    }
}
