// 1.13c D2Client+0x42880 -- wheel-support setup. Globals are passed in (each names its RVA);
// the four user32 imports and the two callees go through D2cHost.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
extern "C" {
// g3cc = 11C3CC, g898 = 119898, g890 = 119890, g88c = 11988C; *msgSlot = F4788.
// Host calls (retail order): imports by IAT slot RVA (CF0E8 GetSystemMetrics(0x4b), CF0E0 RegisterWindowMessageA, CF0EC FindWindowA,
// CF118 SendMessageA), D0B4 (Ordinal_10048, cdecl, s0 = string), 3CCC0 (eax = count, s0 = D0B4 result, s1 = string; ret 8).
EMSCRIPTEN_KEEPALIVE void d2c_42880(D2cHost h, i32 g3cc, i32 g898, i32 g890, i32 g88c, i32* msgSlot) {
    auto call = [&](u32 n, u32 str) {
        D2cCall a = {}; a.rva = 0xD0B4; a.s[0] = 0xB2000000u | str; u32 r = h(&a);
        D2cCall b = {}; b.rva = 0x3CCC0; b.eax = n; b.s[0] = r; b.s[1] = 0xB2000000u | str; h(&b);
    };
    auto imp = [&](u32 rva, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0) {
        D2cCall c = {}; c.rva = rva; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3; return h(&c);
    };
    const bool m45 = g88c == 4 || g88c == 5;
    if (imp(0xCF0E8, 0x4b) != 0 && g3cc != 0 && (g898 == 1 ? g890 != 0 : (g898 == 2 && m45))) call(1, 0xF4748);   // 6faf288f..28c1
    if (g3cc != 0) {                                                                                                // 6faf28d6
        if (g898 == 1 || (g898 == 2 && m45)) call(2, 0xF4754);
        if (g898 == 2 && g88c == 5) call(2, 0xF476C);                                                               // 6faf290e
    }
    if (imp(0xCF0E8, 0x4b) != 0 && g3cc != 0 && g898 == 1 && g890 == 0) {                                          // 6faf2946
        u32 msg = imp(0xCF0E0, 0xB2000000u | 0xD62A8);
        u32 hw = imp(0xCF0EC, 0xB2000000u | 0xD62C0, 0xB2000000u | 0xD62C8);
        if (hw != 0 && msg != 0 && imp(0xCF118, hw, msg, 0, 0) != 0) { *msgSlot = (i32)msg; call(1, 0xF4784); }
    }
}
}
