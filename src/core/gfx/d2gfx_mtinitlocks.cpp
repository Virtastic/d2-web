// 1.13c D2Gfx+0x0191E (__mtinitlocks): MSVC 2003 CRT lock-table init. Walk the 0x24 slots of the
// table at +0x10048 (8-byte records: pointer, flag); each slot flagged 1 gets the next unused
// 0x18-byte critical section from the static block at +0x10E00 and is initialised via
// ___crtInitCritSecAndSpinCount (+0x37CD) with spin count 4000. A failed init zeroes that slot's
// pointer and returns 0; otherwise 1. The slots hold 1.13c image addresses (the table is at 0x6FA90048 and the
// static block at 0x6FA90E00, both from the decompile), so the block is passed as its VA and never dereferenced
// here: the initialiser receives the VA too. Kernel CS init is a host no-op under wasm (GUESS: caller supplies it).
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE
int d2gfx_mtinitlocks(uint32_t* table, uint32_t static_va, int (*init_cs)(uint32_t cs_va, uint32_t spin)) {
    for (int i = 0; i < 0x24; ++i) {
        if (table[i * 2 + 1] == 1) {
            table[i * 2] = static_va;
            static_va += 0x18;
            if (init_cs(table[i * 2], 4000) == 0) {
                table[i * 2] = 0;
                return 0;
            }
        }
    }
    return 1;
}
