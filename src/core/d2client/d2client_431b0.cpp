// 1.13c D2Client+0x431B0 -- the "open server" thread proc, stdcall(1 arg, unused), returns 1 on shutdown.
// Loop: sample (g3d4 stop, g3d8 active) under the critical section; stop -> D060, D066(0,1), DeleteCriticalSection, OutputDebugStringA, return 1.
// Active: D042, ebp = D072(1) (nonzero -> D066(0,0)); with ebp != 0: Sleep(0x1e), then if the player's +0x221 vtable is set and D05A(1) changed,
// compare against g9f58 and call vtable+0x90 (rising) or vtable+0x34(player+0x1B, player+0x241; player+0x259, kind) (falling); else Sleep(10).
// Import calls (Enter/Leave/Delete CriticalSection, Sleep, OutputDebugStringA) go through the host as the IAT slot pseudo-rvas CEFBC/CEFB8/CEFC0/CEFA0/CEF98.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
extern "C" {
EMSCRIPTEN_KEEPALIVE u32 d2c_431b0(D2cHost h, volatile u32* g3d4, volatile u32* g3d8, const uint8_t* const* gplayer, const i32* g9f58) {
    auto call = [&](u32 rva, u32 ecx = 0, u32 edx = 0, u32 s0 = 0, u32 s1 = 0) -> u32 { D2cCall c = {}; c.rva = rva; c.ecx = ecx; c.edx = edx; c.s[0] = s0; c.s[1] = s1; return h(&c); };
    i32 ebx = 0; u32 ebp = 0;
    call(0xCEFBC); u32 esi = *g3d4, edi = *g3d8; call(0xCEFB8);
    for (;;) {
        if (esi) { call(0xD060); call(0xD066, 0, 0, 0, 1); call(0xCEFC0); call(0xCEF98); return 1; }
        bool toSleep10 = false;
        if (edi == 0) {
            // falls to the ebp test below
        } else {
            call(0xD042); ebp = call(0xD072, 0, 0, 1);
            if (ebp == 0) toSleep10 = true; else call(0xD066, 0, 0, 0, 0);
        }
        if (!toSleep10 && ebp == 0) toSleep10 = true;
        if (toSleep10) call(0xCEFA0, 0, 0, 10);
        else {
            call(0xCEFA0, 0, 0, 0x1e);
            const uint8_t* p = *gplayer; u32 vt = *(const u32*)(p + 0x221);
            if (vt) {
                i32 now = (i32)call(0xD05A, 0, 0, 1);
                if (now != ebx) {
                    i32 lim = *g9f58;
                    if (ebx < lim) { if (now >= lim) call(0xFFFF0090 /*vtable+0x90*/); ebx = now; }
                    else {
                        if (now < lim) {
                            u32 k = (p[0x20c] == 1 ? 1u : 0u) | (p[0x20c] == 2 ? 2u : 0u);
                            u32 f = *(const uint16_t*)(p + 0x1eb);
                            if (f & 4) k |= 4;
                            if (f & 0x20) k |= 8;
                            call(0xFFFF0034 /*vtable+0x34*/, (u32)(uintptr_t)p + 0x1b, (u32)(uintptr_t)p + 0x241, (u32)(uintptr_t)p + 0x259, k);
                        }
                        ebx = now;
                    }
                }
            }
        }
        call(0xCEFBC); esi = *g3d4; edi = *g3d8; call(0xCEFB8);
    }
}
}
