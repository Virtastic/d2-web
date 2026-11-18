// D2Client+0x666D0: per-unit action trigger. Callees go through D2cHost (rva | 0x80000000 = ordinal).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
namespace {
inline u32 go(D2cHost h, u32 rva, u32 a = 0, u32 b = 0, u32 c1 = 0, u32 d = 0, u32 e = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = a; c.ecx = a; c.edx = a; c.ebx = e; c.s[0] = a; c.s[1] = b; c.s[2] = c1; c.s[3] = d; return h(&c);
}
inline u32 ord(D2cHost h, u32 n, u32 a = 0, u32 b = 0, u32 c = 0, u32 d = 0) { return go(h, 0x80000000u | n, a, b, c, d); }
inline u32 rd(u32 p, u32 off) { u32 v; memcpy(&v, (const void*)(uintptr_t)(p + off), 4); return v; }
inline void wr(u32 p, u32 off, u32 v) { memcpy((void*)(uintptr_t)(p + off), &v, 4); }
}
extern "C" {
// 1.13c D2Client+0x666D0  (EAX = skipInit, ECX = unit, EBP = mode, EDI = arg for 0x6CC00)
// ponytail: D2cCall carries 4 stack words, so Ordinal_10002's two out-pointers are dropped (args: a, b, 0x60, flag).
EMSCRIPTEN_KEEPALIVE void d2c_666d0(D2cHost h, i32 eax, u32 unit, i32 mode, u32 edi) {
    if (eax == 0) {
        if (unit && ((rd(unit, 0xc8) >> 0x12) & 1)) return;
    }
    if (unit && rd(unit, 0) == 0 && rd(unit, 0x10) == 0x11 && ord(h, 10494, unit, 7) == 0) return;
    if (eax == 0 && unit && (i32)rd(unit, 0) > 0 && (i32)rd(unit, 0) < 3) {
        if (ord(h, 10494, unit, 0x8f) != 0) return;
        if (ord(h, 10494, unit, 0x92) != 0) return;
    }
    u32 a = ord(h, 10013);
    u32 p = ord(h, 11080);
    u32 q = ord(h, 11168, unit);
    if (a != 0 && ord(h, 10012, p, q) == 0) return;
    if (go(h, 0x66620, unit) == 0) return;  // ESI = unit
    if (a == 0) {
        ord(h, 10651, unit);
        ord(h, 11142, unit);
    } else {
        u32 flag = 0;
        if (unit && rd(unit, 0) == 2) {
            u32 i = ord(h, 10688, rd(unit, 4));
            u32 d = ord(h, 10175, unit);
            uint8_t b; memcpy(&b, (const void*)(uintptr_t)(d + 0x131 + i), 1);
            if (b == 2) flag = 1;
        }
        ord(h, 10002, p, q, 0x60, flag);
    }
    auto set = [&] { if (go(h, 0x6CC00, unit, edi) != 0 && unit) wr(unit, 0xc4, rd(unit, 0xc4) | 0x10000000); };
    if (mode == 0) { go(h, 0x728B0); }
    else if (mode == 1) { go(h, 0x4C110, unit); go(h, 0x728B0); }
    else {
        if (mode == 3) { if (go(h, 0x1580) != 0) set(); }
        else set();
        if (unit) ord(h, 10037);
        return;
    }
    set();
    go(h, 0x72830);
    if (unit) ord(h, 10037);
}
}
