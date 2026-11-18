// 1.13c D2Client+0xC7290: EAX = unit. Read off the disassembly: every callee is stdcall;
// the 10123/10331 thunks pop one word each, so the words pushed ahead of them are the trailing arguments of 11073.
// Callees go through D2cHost in retail's order with retail's stack words (s[]) and registers (eax for 0xA7E20).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32;
static u32 rd(u32 a) { return *(const u32*)(uintptr_t)a; }
static u32 call(D2cHost h, u32 rva, u32 s0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0, u32 s4 = 0, u32 eax = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = eax; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3; c.s4 = s4; return h(&c);
}
// 0x1630 / 0x1660 inlined by retail here: kinds 2, 4, 5 read the unit's +0xC / +0x10, the rest ask D2Common
static u32 coord(D2cHost h, u32 self, u32 off, u32 thunk) {
    u32 kind = rd(self), u = rd(self + 0x2c);
    if (kind == 2 || (kind > 3 && kind <= 5)) return rd(u + off);
    return u ? call(h, thunk, u) : 0;
}
extern "C" {
// 1.13c D2Client+0xC7290
EMSCRIPTEN_KEEPALIVE void d2c_c7290(D2cHost h, u32 self) {
    u32 want = rd(self) != 1 ? 0x80u : 0x100u;                    // dec/neg/sbb/and 0xFFFFFF80/add 0x100
    if (call(h, 0xCD24, rd(self + 0x2c)) == want) return;          // D2Common#10259
    u32 r = call(h, 0xC310, self);                                 // #10511
    u32 x = r ? call(h, 0xCB68, r) : coord(h, self, 0xc, 0xC3BE);  // #10980 / #10867
    u32 y = r ? call(h, 0xCB6E, r) : coord(h, self, 0x10, 0xC2EC); // #10649 / #10750
    u32 m = call(h, 0xCD30, self);                                 // #10123 (pops self; `want` stays on the stack)
    u32 room = call(h, 0xC1F0, self);                              // #10331 (pops self; x, y, m stay)
    call(h, 0xCB98, room, x, y, m, want);                          // #11073, ret 0x14
    call(h, 0xA7E20, x, y, 0, 0, 0, self);                         // EAX = unit, ret 0xC
    call(h, 0xC6060, y);
}
}
