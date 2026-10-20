// 1.13c D2Gfx+0x0191E (__mtinitlocks): MSVC 2003 CRT lock-table init. Walk the 0x24 slots of the
// table at +0x10048 (8-byte records: pointer, flag); each slot flagged 1 gets the next unused
// 0x18-byte critical section from the static block at +0x10E00 and is initialised via
// ___crtInitCritSecAndSpinCount (+0x37CD) with spin count 4000. A failed init zeroes that slot's
// pointer and returns 0; otherwise 1. Table, static block and initialiser are seams (GUESS: caller
// supplies the module's own storage; kernel CS init is a host no-op under wasm).
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE
int d2gfx_mtinitlocks(uint32_t* table, uint8_t* static_cs, int (*init_cs)(uint32_t*, uint32_t)) {
    for (int i = 0; i < 0x24; ++i) {
        if (table[i * 2 + 1] == 1) {
            table[i * 2] = (uint32_t)(uintptr_t)static_cs;
            static_cs += 0x18;
            if (init_cs((uint32_t*)(uintptr_t)table[i * 2], 4000) == 0) {
                table[i * 2] = 0;
                return 0;
            }
        }
    }
    return 1;
}
