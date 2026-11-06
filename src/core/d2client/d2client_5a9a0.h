// Seams of D2Client 0x5A9A0 (the per-frame FPS counter and the debug text overlay).
#pragma once
#include <cstdint>
typedef uintptr_t U;
// The counter words live at D2Client+0x11C2AC..: lastCount (+0), lastTick (+4), frames (+8), skip (+0xC), flagTick (+0x10), enable (+0x14), cacheOff (+0x1C).
struct D2Client5a9a0State { uint32_t acc, lastTick, frames, skip, flagTick, enable, pad, cacheOff; };
struct D2Client5a9a0Env {
    uint32_t (*tick)();                                              // GetTickCount (import slot D2Client+0xCEF5C)
    void     (*memStatus)(uint32_t* s8);                             // GlobalMemoryStatus (slot +0xCEFAC); s8[0] = 0x20 on entry, fields at dword 2..5
    void     (*text)(U x, U y, U fmt, U a, U b);                    // thunk 0xD174 (cdecl): fmt is the module RVA of the string; b is the fifth stack word (an argument, or what the caller left there)
    void     (*copy)(char* dst, uint32_t srcRva);                    // 0x3146 (cdecl): strcpy of a module string into the local buffer
    uint32_t (*c10e)();                                              // thunk 0xD10E (video mode)
    uint32_t (*c13c)(); uint32_t (*c148)(uint32_t); uint32_t (*c094)(); uint32_t (*c15a)(); uint32_t (*c14e)(uint32_t);
    uint32_t (*g9810)(); uint32_t (*g9804)(); uint32_t (*c040)(); uint32_t (*c044)(); uint32_t (*c048)();
    void     (*cacheOverlay)();                                      // 0x5A680
};
// 1.13c D2Client+0x5A9A0
void d2c_5a9a0(const D2Client5a9a0Env& e, D2Client5a9a0State* s);
