// 1.13c D2Client+0xA86C0 (59 B, ret 0xC): EAX = light record, stack = three colour words (only the low byte of each is stored).
// Stores them at record+0x25, +0x26, +0x27. A null record takes the fatal path (Ordinal_10265(0x167), Ordinal_10024, _exit(-1)).
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0xA86C0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a86c0(uintptr_t rec, uint32_t c0, uint32_t c1, uint32_t c2, void (*fatal)(uint32_t)) {
    if (rec == 0) { fatal(0x167); return; }
    *(uint8_t*)(rec + 0x25) = (uint8_t)c0;
    *(uint8_t*)(rec + 0x26) = (uint8_t)c1;
    *(uint8_t*)(rec + 0x27) = (uint8_t)c2;
}
