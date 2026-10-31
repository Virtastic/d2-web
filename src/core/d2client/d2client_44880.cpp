// 1.13c D2Client+0x44880 -- sets DAT_6FBA46C4 := 8; with 11C3A0 && 11C3DC && !11C3E4: 11C3F8 := 1,
// flags := 0x427C0 (inlined: ((e8||ec) ? 8 : 0) | (f0 ? 4 : 0)), edx := the pointer global 11BBFC; if non-null and *edx == 0 -> 1BF20(1, 11B828, 11B824, flags) (stdcall 4).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32;
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2c_44880(D2cHost h, u32* g46c4, u32 g3a0, u32 g3dc, u32 g3e4, u32* g3f8, u32 g3e8, u32 g3ec, u32 g3f0, const u32* edx, u32 gb828, u32 gb824) {
    *g46c4 = 8;
    if (g3a0 && g3dc && !g3e4) {
        *g3f8 = 1;
        u32 flags = ((g3e8 ? 8u : 0u) | (g3ec ? 8u : 0u)) | (g3f0 ? 4u : 0u);   // 1.13c D2Client+0x427C0
        if (edx && *edx == 0) { D2cCall c = {}; c.rva = 0x1BF20; c.s[0] = 1; c.s[1] = gb828; c.s[2] = gb824; c.s[3] = flags; h(&c); }
    }
}
}
