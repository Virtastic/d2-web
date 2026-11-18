// Trade-action wrapper, exact register/stack interface from the disassembly. Callee through D2cHost (client_host.h).
#include "client_host.h"
#include <emscripten.h>

struct D2c95f80G { uint32_t bcc34, b834, b864, b854, b858; };   // DAT_6FBCBC34, 6FBCB834, 6FBCB864, 6FBCB854, 6FBCB858

// 1.13c D2Client+0x95F80: ECX = unit, EDX = edx, EAX = eaxIn, ESI = esi, stack (arg1, arg2, arg3) ret 0xC.
// ecx == 0: return. 0x47D60 runs with EAX = arg3 and stack (arg1, arg2, edx, eaxIn, esi, 0); zero result: return.
// Otherwise esi == 0: bcc34 = 5; then b834 = 0, b864 = 1, b854 = 5, b858 = 0.
// (The earlier port took the call as (b, c, a); the stack order is arg1, arg2 and EAX carries arg3.)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_95f80(D2cHost h, uint32_t ecx, uint32_t edx, uint32_t eaxIn, uint32_t esi,
                                                    uint32_t arg1, uint32_t arg2, uint32_t arg3, D2c95f80G* g) {
    if (ecx == 0) return;
    D2cCall c = { 0x47D60, arg3, ecx, 0, 0, esi, { arg1, arg2, edx, eaxIn } };   // trailing stack words: esi, 0
    if (h(&c) == 0) return;
    if (esi == 0) g->bcc34 = 5;
    g->b834 = 0; g->b864 = 1; g->b854 = 5; g->b858 = 0;
}
