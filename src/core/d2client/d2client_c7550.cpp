// 1.13c D2Client+0xC7550: ECX = unit, EDX = skill id, one unread stack word (ret 4).
// Read off the disassembly. player = the word at [0x11BBFC]. Callees go through D2cHost with retail's registers and
// stack words; 10566's two out-words are stack locals zeroed before the call, as in retail.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32;
static u32 rd(u32 a) { return *(const u32*)(uintptr_t)a; }
namespace {
u32 hc(D2cHost h, u32 rva, u32 eax, u32 ecx, u32 edx, u32 edi, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0, u32 s4 = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = eax; c.ecx = ecx; c.edx = edx; c.edi = edi;
    c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3; c.s4 = s4; return h(&c);
}
u32 st(D2cHost h, u32 rva, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0, u32 s4 = 0) { return hc(h, rva, 0, 0, 0, 0, s0, s1, s2, s3, s4); }
const u32 kSrc = 0xB2000000u | 0xD5F6C;   // ..\Source\D2Client\SKILLS\SkillsBar.cpp (module pointer: the capture records it as 0xB2000000|rva)
}
extern "C" {
// 1.13c D2Client+0xC7550
EMSCRIPTEN_KEEPALIVE u32 d2c_c7550(D2cHost h, u32 unit, u32 skill, u32 player) {
    u32 rec = hc(h, 0xC838, 0, unit, skill, 0);                    // D2Common#10630 (fastcall)
    if (!rec) return 0;
    if (unit == player) {
        if (st(h, 0xC1FC, unit, 0x36)) return 0;                                             // #10494
        if (st(h, 0xC42A, rec, kSrc, 0x3a2) == skill && (st(h, 0xC628, rec) & 1)) return 0;   // #10601, #10397
    }
    hc(h, 0x2A0B0, unit, 0, 0, 0);
    u32 tg = rd(unit + 0x2c);
    if (!tg) {                                                      // fatal: never returns in retail
        u32 f = st(h, 0xBF92, 0x946); st(h, 0xBF80, 0xB2000000u | 0xD359F, f); st(h, 0x3664, 0xffffffffu); return 0;
    }
    u32 t = st(h, 0xC2DA, tg);                                      // #10392
    u32 xy[2] = { 0, 0 };
    if (!st(h, 0xCD3C, unit, t, (u32)(uintptr_t)&xy[0], (u32)(uintptr_t)&xy[1])) {   // #10566
        if (t && st(h, 0xC6E2, unit, t, 0)) return hc(h, 0xA3040, t, 0, 0, 0, unit);   // #11138
        if (rd(unit) != 1) st(h, 0x23200, unit, 0x13);
        return 0;
    }
    u32 room = st(h, 0xC1F0, unit);                                 // #10331
    if (!room) return 0;
    u32 m = st(h, 0xCD30, unit);                                    // #10123
    st(h, 0xCC8E, room, xy[0], xy[1], m, 0x80);                     // #10565, ret 0x14
    st(h, 0xCB50, rec, xy[0]);                                      // #10243
    st(h, 0xCB7A, rec, xy[1]);                                      // #10554
    if (t) { st(h, 0xCB8C, rec, rd(t)); st(h, 0xC74E, rec, rd(t + 0xc)); }   // #10748, #11031
    else st(h, 0xCB8C, rec, 6);
    st(h, 0xC60A, rec, 0x1080);                                     // #10137
    hc(h, 0xA3220, 0, 0, 0, unit);
    return 1;
}
}
