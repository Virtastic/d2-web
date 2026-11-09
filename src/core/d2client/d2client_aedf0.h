// 1.13c D2Client+0xAEDF0 env seam, shared by the port (d2client_aedf0.cpp) and its FN probe.
#pragma once
#include <cstdint>

struct D2cAEDF0Env {
    void* hero;                                                              // global 0x11BBFC: the hero unit
    int32_t (*getStat)(void* unit, int32_t stat, int32_t zero);              // D2Client+0xC1C6 -> D2Common #10973 (stdcall, 3 words)
    void (*setStat)(void* unit, int32_t stat, int32_t value, int32_t zero);  // D2Client+0xC40C -> D2Common #10887 (stdcall, 4 words)
    void (*statEvent)(int32_t stat, void* unit, int32_t value);              // D2Client+0xAEB10: EAX = stat, ECX = unit, stack = value
    void (*fatal)(void);                                                     // 0xBF80 / 0xBF92 / 0x3664: null hero, does not return
};

extern "C" void d2c_aedf0(const D2cAEDF0Env* e, const uint8_t* packet);
