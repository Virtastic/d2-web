// D2Client 0x45E50: the hireling popup open (ECX = id; caller 0x62E60).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
// 1.13c D2Client+0x45E50 ; the id scan over the 0x10-byte entries 0x11967D..0x11971C has no effect (its result is never read).
// gc284 = the dword at 0x11C284, gc29c = the byte at 0x11C29C. Callees through host: 0xC2790 (ECX = 0x23, EDX = 0, one stack word 0),
// 0xBE8A (Ordinal_423: "Diablo II" 0x6FB83588, "PopupHireling" 0x6FB8566C, 0, &out; host returns the written value in out[0]).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_45e50(uint32_t* gc284, uint8_t* gc29c, D2cHost host) {
    *gc284 = 0;
    D2cCall c = {}; c.rva = 0xC2790; c.ecx = 0x23; host(&c);
    c = {}; c.rva = 0xBE8A; c.s[0] = 0x6FB83588u; c.s[1] = 0x6FB8566Cu; host(&c);
    if (c.out[0] == 0) *gc29c = 1;
}
