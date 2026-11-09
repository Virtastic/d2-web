// 1.13c D2Client+0xC76C0: ECX = unit, EDX = skill id, [esp+4] = arg (ret 4; read only by
// the non-monster path, handed to 0xC66A0). Read off the disassembly. player = the word at [0x11BBFC]. Callees go through
// D2cHost with retail's registers and stack words; out-pointers are stack locals as in retail (x at the EDI out, y at word 0).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32;
static u32 rd(u32 a) { return *(const u32*)(uintptr_t)a; }
namespace {
struct R { u32 eax = 0, ecx = 0, ebx = 0, esi = 0, edi = 0; };
u32 hc(D2cHost h, u32 rva, R r, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0, u32 s4 = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = r.eax; c.ecx = r.ecx; c.ebx = r.ebx; c.esi = r.esi; c.edi = r.edi;
    c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3; c.s4 = s4; return h(&c);
}
u32 st(D2cHost h, u32 rva, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0, u32 s4 = 0) { return hc(h, rva, R{}, s0, s1, s2, s3, s4); }
u32 rg(D2cHost h, u32 rva, u32 eax, u32 ecx, u32 esi) { R r; r.eax = eax; r.ecx = ecx; r.esi = esi; return hc(h, rva, r); }
u32 p(const void* a) { return (u32)(uintptr_t)a; }
const u32 kSrc = 0xB2000000u | 0xD5F6C;   // ..\Source\D2Client\SKILLS\SkillsBar.cpp (module pointer: the capture records it as 0xB2000000|rva)
}
extern "C" {
// 1.13c D2Client+0xC76C0
EMSCRIPTEN_KEEPALIVE u32 d2c_c76c0(D2cHost h, u32 unit, u32 skill, u32 arg, u32 player) {
    u32 rec = st(h, 0xC310, unit);                                  // D2Common#10511
    if (!rec) return 0;
    if (unit == player) {
        if (st(h, 0xC1FC, unit, 0x36)) return 0;                                             // #10494
        if (st(h, 0xC42A, rec, kSrc, 0x1ba) == skill && (st(h, 0xC628, rec) & 1)) return 0;   // #10601, #10397
    }
    u32 k1 = (unit ? rd(unit) : 6u) == 1;
    u32 x, y;
    if (k1) {
        u32 t = rg(h, 0xA5F70, 0, 0, unit);
        if (!t) {
            u32 oy = 0, ox = 0; R r; r.ebx = unit; r.edi = p(&ox);
            if (!hc(h, 0xA1340, r, p(&oy))) return 0;
            st(h, 0xC74E, rec, 0xffffffffu);                        // #11031
            x = ox; y = oy;
        } else {
            u32 tx = rg(h, 0x1630, 0, t, 0), ty = rg(h, 0x1660, 0, t, 0);
            x = 2 * tx - rg(h, 0x1630, 0, unit, 0);
            y = 2 * ty - rg(h, 0x1660, 0, unit, 0);
        }
        u32 xy[2] = { x, y };
        u32 v = st(h, 0xC4C0, unit);                                // #10589
        u32 room = st(h, 0xC1F0, unit);                             // #10331
        if (!st(h, 0xC676, room, p(xy), v, 0x3c01, 0)) return 0;    // #10509, ret 0x14
        x = xy[0]; y = xy[1];
    } else {
        u32 oy = 0, ox = 0; R r; r.ebx = unit; r.edi = p(&ox);
        if (!hc(h, 0xA1340, r, p(&oy))) return 0;
        r = R{}; r.eax = skill;
        if (!hc(h, 0xC5FE0, r, unit, ox)) return 0;                 // ret 8
        if (!hc(h, 0xC66A0, r, unit, arg, p(&ox), p(&oy))) return 0;   // ret 0x10
        x = ox; y = oy;
    }
    R r; r.eax = st(h, 0xC1F0, unit); r.ebx = x; r.edi = y;
    u32 q = hc(h, 0x1400, r);
    u32 m = st(h, 0xCD30, unit);                                    // #10123
    st(h, 0xCC8E, q, x, y, m, 0x80);                                // #10565, ret 0x14
    st(h, 0xCB50, rec, x);                                          // #10243
    st(h, 0xCB7A, rec, y);                                          // #10554
    if (k1) {
        u32 t = rg(h, 0xA5F70, 0, 0, unit);
        if (t) { st(h, 0xCB8C, rec, rd(t)); st(h, 0xC74E, rec, rd(t + 0xc)); }   // #10748, #11031
    }
    st(h, 0xC60A, rec, 0x1080);                                     // #10137
    r = R{}; r.edi = unit; hc(h, 0xA3220, r);
    return 1;
}
}
