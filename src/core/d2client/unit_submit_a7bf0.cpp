// 1.13c D2Client+0xA7BF0 (193 B, ret 0xC): ECX = p1, EDX = p2, stack s0, type, s2. Bumps the counter 0xDE834 (0xFFFFFFFF wraps to 0). type 1 builds the 7-word record {counter, p1, p2, s0, 0, 0, type}
// on the stack and calls 0xA7780 (stack: &record, 0, 0, 0, 0, type; ret 0x18). Otherwise 0xC69A (stdcall 0, type) returns a unit (null returns); its +0xC4 |= 0x600000 and 0xA7400 runs
// with EAX = p2, ECX = unit, EDX = counter and stack p1, s0, type, s2, 0 (top first).
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a7bf0(uint32_t p1, uint32_t p2, uint32_t s0, uint32_t type, uint32_t s2, uint32_t* counter,
                                                     void (*f7780)(const uint32_t* rec, uint32_t z0, uint32_t z1, uint32_t z2, uint32_t z3, uint32_t type),
                                                     uintptr_t (*fc69a)(uint32_t z, uint32_t type),
                                                     void (*f7400)(uint32_t eax, uintptr_t ecx, uint32_t edx, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e)) {
    uint32_t c = *counter + 1;
    *counter = c;
    if (c == 0xffffffffu) { c = 0; *counter = 0; }
    if (type == 1) {
        const uint32_t rec[7] = {c, p1, p2, s0, 0, 0, type};
        f7780(rec, 0, 0, 0, 0, type);
        return;
    }
    uintptr_t u = fc69a(0, type);
    if (!u) return;
    *(uint32_t*)(u + 0xc4) |= 0x600000;
    f7400(p2, u, *counter, p1, s0, type, s2, 0);
}
