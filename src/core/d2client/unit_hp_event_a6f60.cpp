// 1.13c D2Client+0xA6F60 (90 B): ESI = unit, EAX = amount, ECX = code. A null unit returns. Calls 0x6B300 (EAX = code, 7 stack words: unit, k, 0, amount, 0, 0, 0
// with k = 1 for amount > 0, else k = 2 and the amount word 0), then for code 0x97 / 0x98 calls 0x88A70 (EBX = 0x18C / 0x18D, stack unit 0 0 0).
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a6f60(uintptr_t unit, int32_t amt, int32_t code, void (*f1b300)(int32_t eax, uintptr_t unit, uint32_t k, uint32_t z0, int32_t amt),
                                                     void (*f88a70)(uint32_t ebx, uintptr_t unit)) {
    if (!unit) return;
    if (amt > 0) f1b300(code, unit, 1, 0, amt);
    else f1b300(code, unit, 2, 0, 0);
    if (code == 0x97) f88a70(0x18c, unit);
    else if (code == 0x98) f88a70(0x18d, unit);
}
