// 1.13c D2Client+0x13FD0 (113 B): stdcall(a0, a1, a2, a3, a4), ret 0x14. Calls 0x13F40 three times with
// EAX = a0 and the register pair (ESI, EDI) stepping over a 5x5 grid of the two coordinates:
//   bx = 5 * (a1 / 5), by = 5 * (a2 / 5)   (signed, truncating)
//   call 1: ESI = by,            EDI = bx
//   call 2: ESI = by + a4,       EDI = bx + a3
//   call 3: ESI = by + 2 * a4,   EDI = bx + 2 * a3
// EBX holds bx throughout (the callee's own frame reads it). The decompile drops every one of these arguments.
#include <cstdint>
#include <emscripten.h>

struct D2Client13fd0Env {
    void (*call_13f40)(uint32_t eax, uint32_t ebx, uint32_t esi, uint32_t edi);
};

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_13fd0(int32_t a0, int32_t a1, int32_t a2, int32_t a3, int32_t a4, const D2Client13fd0Env* env) {
    const uint32_t bx = (uint32_t)(5 * (a1 / 5));
    const uint32_t by = (uint32_t)(5 * (a2 / 5));
    env->call_13f40((uint32_t)a0, bx, by, bx);
    env->call_13f40((uint32_t)a0, bx, (uint32_t)a4 + by, bx + (uint32_t)a3);
    env->call_13f40((uint32_t)a0, bx, by + 2u * (uint32_t)a4, bx + 2u * (uint32_t)a3);
}
