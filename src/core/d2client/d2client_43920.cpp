// 1.13c D2Client+0x43920 -- net message pump.
// Drains Ordinal_10020 into a 512-byte stack buffer; byte0 0xAF/0xB0 set/clear DAT_6FBCC39C (11C39C), 0xB3 -> ACD70 (esi=buf),
// 0xB4 -> ACBF0 (eax=buf); Sleep(0) each pass (an import, not a host call); then drains Ordinal_10008 feeding ACE20 (eax=buf, push n).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2c_43920_run(D2cHost h, uint8_t* buf /*512 bytes*/, i32* g39c) {
    const u32 B = 0xB3000000u;   // the buffer is a stack local in retail
    i32 n;
    do {
        D2cCall a = {}; a.rva = 0xD210; a.s[0] = (u32)(uintptr_t)buf; a.s[1] = 0x200; n = (i32)h(&a);
        if (n != -1) {
            switch (buf[0]) {
            case 0xaf: *g39c = 1; break;
            case 0xb0: *g39c = 0; break;
            case 0xb3: { D2cCall c = {}; c.rva = 0xACD70; c.esi = B; h(&c); break; }
            case 0xb4: { D2cCall c = {}; c.rva = 0xACBF0; c.eax = B; h(&c); break; }
            default: break;
            }
        }
    } while (n != -1);
    do {
        D2cCall a = {}; a.rva = 0xD24C; a.s[0] = (u32)(uintptr_t)buf; a.s[1] = 0x200; n = (i32)h(&a);
        D2cCall c = {}; c.eax = B; c.rva = 0xACE20; c.s[0] = (u32)n; h(&c);
    } while (n != -1);
}
}
