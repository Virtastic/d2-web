// src/core/d2win/d2win_16360.cpp -- 1.13c D2Win.dll+0x16360: type-8 timer control, interval tick against a shared next-tick global.
// fastcall, ECX = the control; stdcall callback word at +0x34.
#include <cstdint>
#include <cstdlib>

// fatal: the type-check failure report (retail: string lookup, error report, _exit(-1)); nextTick: DAT 0xCA674; tick: GetTickCount;
// call: invoke the control's callback (+0x34, a stdcall word) with the address of a one-word block holding the control.
// 1.13c D2Win+0x16360
extern "C" uint32_t d2win_16360(uint32_t* w, uint32_t* nextTick, uint32_t (*tick)(), void (*fatal)(uint32_t line),
                                void (*call)(uint32_t cb, uint32_t* block)) {
    if (w[0] != 8) { fatal(0x6a); std::exit(-1); }
    if (!((1u << 2) & w[2]) || !((1u << 0) & w[2])) return 0;   // gdwBitMasks[2], gdwBitMasks[0]
    uint32_t now = tick();
    if (now >= *nextTick) {
        uint32_t block[8];
        block[0] = (uint32_t)(uintptr_t)w;
        if (w[0xd]) call(w[0xd], block);
        *nextTick = w[0x10] + now;
    }
    return 1;
}
