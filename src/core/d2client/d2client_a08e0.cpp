// stack arg 0 = unit (ret 4). For every skill id in the
// data-table list (dt+0x184, count int16 dt+0x188) the unit has (10494), the skill row's handler index (int16 +0x34 of the
// 0x3C-byte row at dt+0xBC, count dt+0xC4) selects a callback in the module table 0xDEA48 (count 0xDEC50); it runs with
// ECX = unit, EDX = stat 0x15E and stack = stat 0x15F of the unit's skill record (0 / 1 without one).
#include <cstdint>
#include "cl_env.h"
namespace { constexpr uint32_t kBase = 0x6FAB0000u; }
// 1.13c D2Client+0xA08E0
extern "C" void d2c_a08e0(uint32_t unit, const cl::Env& e) {
    if (!e.H(0xc78a, unit)) return;                                 // 10276
    for (int32_t i = 0; i < e.R16s(e.dt + 0x188); ++i) {
        const int32_t sk = e.R16s(e.R(e.dt + 0x184) + 2u * (uint32_t)i);
        if (!e.H(0xc1fc, unit, (uint32_t)sk)) continue;             // 10494
        if (sk < 0 || sk >= (int32_t)e.R(e.dt + 0xc4)) continue;
        const uint32_t row = e.R(e.dt + 0xbc) + (uint32_t)sk * 0x3c;
        if (!row) continue;
        const int32_t idx = e.R16s(row + 0x34);
        if (idx < 0 || idx >= (int32_t)e.R(kBase + 0xdec50)) continue;
        const uint32_t fn = e.R(kBase + 0xdea48 + 4u * (uint32_t)idx);
        if (!fn) continue;
        uint32_t a = 0, b = 1;
        const uint32_t sd = e.H(0xc1c0, unit, (uint32_t)sk);        // 10871
        if (sd) { a = e.H(0xc4b4, sd, 0x15e, 0); b = e.H(0xc4b4, sd, 0x15f, 0); }   // 10680
        const uint32_t rva = (fn >> 24) == 0xB2u ? fn & 0xFFFFFFu : fn - kBase;
        e.H(rva, b, 0, 0, 0, 0, 0, unit, a);
    }
}
