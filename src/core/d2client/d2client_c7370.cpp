// 1.13c D2Client+0xC7370: ECX = unit, EDX = skill id, one unread stack word (ret 4).
// Read off the disassembly. player = the word at [0x11BBFC]. Callees go through D2cHost with retail's registers and
// stack words; the out-pointers (0xA1340's two, 10817's table) are stack locals here as in retail.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32;
typedef int32_t i32;
static u32 rd(u32 a) { return *(const u32*)(uintptr_t)a; }
static u32 rd16(u32 a) { return *(const uint16_t*)(uintptr_t)a; }
static i32 rd16s(u32 a) { return *(const int16_t*)(uintptr_t)a; }
static u32 rd8(u32 a) { return *(const uint8_t*)(uintptr_t)a; }
namespace {
struct R { u32 eax = 0, ecx = 0, edx = 0, ebx = 0, esi = 0, edi = 0; };
u32 hc(D2cHost h, u32 rva, R r, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0, u32 s4 = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = r.eax; c.ecx = r.ecx; c.edx = r.edx; c.ebx = r.ebx; c.esi = r.esi; c.edi = r.edi;
    c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3; c.s4 = s4; return h(&c);
}
u32 st(D2cHost h, u32 rva, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0, u32 s4 = 0) { return hc(h, rva, R{}, s0, s1, s2, s3, s4); }
const u32 kSrc = 0xB2000000u | 0xD5F6C;   // ..\Source\D2Client\SKILLS\SkillsBar.cpp (module pointer: the capture records it as 0xB2000000|rva)
}
extern "C" {
// 1.13c D2Client+0xC7370
EMSCRIPTEN_KEEPALIVE u32 d2c_c7370(D2cHost h, u32 unit, u32 skill, u32 player) {
    if (!unit) return 0;
    if (rd(unit) != 0 && rd(unit) != 1) return 0;
    R r; r.ecx = unit; r.edx = skill;
    u32 rec = hc(h, 0xC838, r);                                    // D2Common#10630 (fastcall)
    u32 tg = rd(unit + 0x2c);
    if (!rec || !tg) return 0;
    if (unit == player) {
        if (st(h, 0xC42A, rec, kSrc, 0x469) == skill && (st(h, 0xC628, rec) & 1)) return 0;   // #10601, #10397
        if (st(h, 0xC1FC, unit, 0x36)) return 0;                                            // #10494
    }
    u32 o0 = 0, o1 = 0;
    r = R{}; r.ebx = unit; r.edi = (u32)(uintptr_t)&o1;
    if (!hc(h, 0xA1340, r, (u32)(uintptr_t)&o0)) return 0;
    r = R{}; r.esi = unit;
    u32 t = hc(h, 0xA5F70, r);
    if (t && st(h, 0xC6E2, unit, t, 0)) { r = R{}; r.eax = t; return hc(h, 0xA3040, r, unit); }   // #11138
    u32 k = rd(unit) == 0, fail = k ? 0x1C09u : 0x3C01u, mode = k ? 0x80u : 0x100u;
    st(h, 0xC3AC, tg, 0xc01);                                      // #11164
    st(h, 0xC38E, tg, 7);                                          // #10647
    if (!st(h, 0xC376, tg, unit, 0)) { st(h, 0xC3AC, tg, fail); return 0; }   // #10334
    r = R{}; r.eax = rd(unit + 4);
    if (k) { u32 e = hc(h, 0x1300, r); if (e) st(h, 0xC328, tg, rd8(e + 0x40) << 8, kSrc, 0x499); }
    else { u32 e = hc(h, 0x1230, r); st(h, 0xC328, tg, (u32)rd16s(e + 0x32) << 8, kSrc, 0x49f); }   // #10488
    st(h, 0xC3AC, tg, 0x401);
    u32 tbl = 0;
    u32 n = st(h, 0xCD18, tg, (u32)(uintptr_t)&tbl);               // #10817
    if (!n) { st(h, 0xC3AC, tg, fail); return 0; }
    u32 x = rd16(tbl + n * 4 - 4), y = rd16(tbl + n * 4 - 2);
    u32 m = st(h, 0xCD30, unit);                                   // #10123
    u32 room = st(h, 0xC1F0, unit);                                // #10331
    st(h, 0xCC8E, room, x, y, m, mode);                            // #10565, ret 0x14
    st(h, 0xC60A, rec, 1);                                         // #10137
    st(h, 0xCB50, rec, x);                                         // #10243
    st(h, 0xCB7A, rec, y);                                         // #10554
    r = R{}; r.edi = unit; hc(h, 0xA3220, r);
    return 1;
}
}
