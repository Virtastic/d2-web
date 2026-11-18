// 1.13c D2Client+0x42C10 -- x position that centres the D2Lang string `id` on `x` (stdcall (id, x), ret 8).
// Callees through D2cHost: IAT CEB00 (Unicode default ctor, 100 times over the 200-byte local), D1EC (ecx = id -> string), IAT CEAB8
// (Unicode::strcpy, ecx = local, edx = string), D35A (ecx = local -> pixel width). The local buffer is a stack address in retail (0xB3000000 here).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
extern "C" {
EMSCRIPTEN_KEEPALIVE i32 d2c_42c10(D2cHost h, u32 id, i32 x) {
    for (int i = 0; i < 100; ++i) { D2cCall c = {}; c.rva = 0xCEB00; c.ecx = 0xB3000000u; h(&c); }
    D2cCall s = {}; s.rva = 0xD1EC; s.ecx = id; u32 str = h(&s);
    D2cCall cp = {}; cp.rva = 0xCEAB8; cp.ecx = 0xB3000000u; cp.edx = str; h(&cp);
    D2cCall w = {}; w.rva = 0xD35A; w.ecx = 0xB3000000u; i32 width = (i32)h(&w);
    return x - ((width + (width < 0)) >> 1);   // cdq; sub eax,edx; sar 1 = signed width / 2
}
}
