// src/core/d2win/d2win_163e0.cpp -- 1.13c D2Win.dll+0x163E0: the type-8 timer control's per-second poll (installed at +0x20 by the
// constructor at 0x164D0). fastcall, ECX = the control.
#include <cstdint>
#include <cstdlib>

// fatal: the type-check failure report (retail: string lookup, error report, _exit(-1)); tick: GetTickCount;
// call: invoke the control's callback (+0x34, a stdcall word) with the address of a one-word block holding the control.
// 1.13c D2Win+0x163E0
extern "C" uint32_t d2win_163e0(uint32_t* w, uint32_t (*tick)(), void (*fatal)(uint32_t line),
                                void (*call)(uint32_t cb, uint32_t* block)) {
    if (w[0] != 8) { fatal(0x52); std::exit(-1); }
    if (!((1u << 2) & w[2]) || !((1u << 0) & w[2])) return 0;   // gdwBitMasks[2], gdwBitMasks[0]
    uint32_t now = tick();
    if (now / 1000 - w[0x11] / 1000 > w[0x10]) {                // unsigned, wraps as retail's sub/cmp/jbe does
        uint32_t block[8];
        block[0] = (uint32_t)(uintptr_t)w;
        if (w[0xd]) call(w[0xd], block);
    }
    return 1;
}
