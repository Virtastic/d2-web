// 1.13c D2Client+0x429C0 -- the wheel-support string hand-off without the MouseZ probe.
// Same shape as 0x42880 but the callee is 0x3CC60 and the last block is gated on F46D4 instead of the FindWindow/SendMessage probe.
// Globals are passed in; GetSystemMetrics (IAT slot CF0E8), D0B4 and 3CC60 go through D2cHost.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
extern "C" {
// g3cc = 11C3CC, g898 = 119898, g890 = 119890, g88c = 11988C, g46d4 = F46D4.
EMSCRIPTEN_KEEPALIVE void d2c_429c0(D2cHost h, i32 g3cc, i32 g898, i32 g890, i32 g88c, i32 g46d4) {
    auto call = [&](u32 n, u32 str) {
        D2cCall a = {}; a.rva = 0xD0B4; a.s[0] = 0xB2000000u | str; u32 r = h(&a);
        D2cCall b = {}; b.rva = 0x3CC60; b.eax = n; b.s[0] = r; b.s[1] = 0xB2000000u | str; h(&b);
    };
    auto wheel = [&]() { D2cCall c = {}; c.rva = 0xCF0E8; c.s[0] = 0x4b; return h(&c) != 0; };
    const bool m45 = g88c == 4 || g88c == 5;
    if (wheel() && g3cc != 0 && (g898 == 1 ? g890 != 0 : (g898 == 2 && m45))) call(1, 0xF4748);   // 6faf29c9..2a11
    if (g3cc != 0) {                                                                                // 6faf2a16
        if (g898 == 1 || (g898 == 2 && m45)) call(2, 0xF4754);
        if (g898 == 2 && g88c == 5) call(2, 0xF476C);                                               // 6faf2a4e
    }
    if (wheel() && g3cc != 0 && g898 == 1 && g890 == 0 && g46d4 != 0) call(1, 0xF4784);             // 6faf2a86
}
}
