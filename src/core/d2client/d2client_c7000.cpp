// 1.13c D2Client+0xC7000: ECX = unit, EDX = skill row index, one stack word (ret 4).
// Read off the disassembly. dtv = the word at [0xCE474] (D2Common's sgptDataTables variable, read through twice),
// bm = the word at [0xCEE50] (gdwBitMasks). Callees go through D2cHost with retail's registers and stack words.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32;
typedef int32_t i32;
static u32 rd(u32 a) { return *(const u32*)(uintptr_t)a; }
static i32 rd16s(u32 a) { return *(const int16_t*)(uintptr_t)a; }
static u32 rd8(u32 a) { return *(const uint8_t*)(uintptr_t)a; }
static u32 hc(D2cHost h, u32 rva, u32 eax, u32 ecx, u32 ebx, u32 edi, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0, u32 s4 = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = eax; c.ecx = ecx; c.ebx = ebx; c.edi = edi;
    c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3; c.s4 = s4; return h(&c);
}
extern "C" {
// 1.13c D2Client+0xC7000
EMSCRIPTEN_KEEPALIVE u32 d2c_c7000(D2cHost h, u32 unit, u32 idx, u32 arg3, u32 dtv, u32 bm) {
    if ((i32)idx < 0) return 0;
    u32 dt = rd(dtv);
    if ((i32)idx >= (i32)rd(dt + 0xba0)) return 0;
    u32 row = idx * 0x23c + rd(dt + 0xb98);
    if (!row) return 0;
    hc(h, 0x2A0B0, unit, 0, 0, 0);
    u32 tg = rd(unit + 0x2c);
    if (!tg) {                                                    // fatal: never returns in retail
        u32 f = 0; D2cCall c = {}; c.rva = 0xBF92; c.s[0] = 0x946; f = h(&c);
        c = {}; c.rva = 0xBF80; c.s[0] = 0x6fb8359f; c.s[1] = f; h(&c);
        c = {}; c.rva = 0x3664; c.s[0] = 0xffffffffu; h(&c);
        return 0;
    }
    u32 p = hc(h, 0xC2DA, 0, 0, 0, 0, tg);                         // D2Common#10392
    if (!p) return 0;
    if (!hc(h, 0xCD1E, 0, 0, 0, 0, p)) return 0;                   // #10338
    *(u32*)(uintptr_t)(unit + 0xc4) = rd(unit + 0xc4) | 0x40;
    u32 xy[2];
    xy[0] = hc(h, 0x1630, 0, p, 0, 0);
    xy[1] = hc(h, 0x1660, 0, p, 0, 0);
    u32 room = hc(h, 0xC1F0, 0, 0, 0, 0, p);                       // #10331
    if (!room) return 0;
    u32 q = hc(h, 0x1400, room, 0, xy[0], xy[1]);
    if (!q) return 0;
    if (!hc(h, 0xC676, 0, 0, 0, 0, q, (u32)(uintptr_t)xy, 2, 0x1000, 1)) return 0;   // #10509, ret 0x14
    if (rd(p) != 1) return 0;
    i32 e = rd16s(row + 0xea);
    u32 t = hc(h, 0x1260, rd(p + 4), 0, 0, 0);
    if (!t) return 0;
    u32 b5 = rd8(t + 5);
    if (rd(bm + 0xc) & b5) e = rd16s(row + 0xee);
    else if (rd(bm + 8) & b5) e = rd16s(row + 0xec);
    if (e < 0 || e >= (i32)rd(rd(dtv) + 0xb6c)) return 0;
    u32 rec[23] = {};
    rec[0] = 0x2001; rec[1] = unit; rec[4] = (u32)e; rec[5] = xy[0]; rec[6] = xy[1]; rec[11] = idx; rec[12] = arg3;
    hc(h, 0xB30F0, 0, (u32)(uintptr_t)rec, 0, 0);
    rec[4] = 0x73;
    hc(h, 0xB30F0, 0, (u32)(uintptr_t)rec, 0, 0);
    return 1;
}
}
