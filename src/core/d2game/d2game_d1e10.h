// 1.13c D2Game+0xD1E10 : fill the 0x84-byte out record (ESI) from the source record (EBX) and the unit (stack argument).
// The body is a template over its four callees so the engine (weak Ord10086/Ord10867/Ord10750/Ord10193 in d2game_batch_b65.cpp)
// and the FN probe (src/core/fn/d2game_d1e10.cpp, recorder hooks) run the same code.
// Callee arguments as retail pushes them: #10086 none, #10867 / #10750 (the +0x2c pointer), #10193 (EBX).
#pragma once
#include <cstdint>

template <class Ord10086, class Ord10867, class Ord10750, class Ord10193>
static inline uint32_t d2game_d1e10_body(intptr_t ebx, uint32_t* esi, intptr_t p1, intptr_t p2, Ord10086 o10086, Ord10867 o10867, Ord10750 o10750, Ord10193 o10193)
{
    for (int i = 0; i < 0x21; i++) esi[i] = 0;
    esi[2] = (uint32_t)p1;
    esi[3] = (uint32_t)o10086(ebx);
    *(uint16_t*)((uint8_t*)esi + 0x2a) = *(uint16_t*)(p1 + 0x78);
    esi[5] = (uint32_t)p2;   // the second stack argument (retail reads [esp+0x10] after the #10086 call pops its word)
    esi[6] = 4;
    int t = (int)*(uint32_t*)(ebx + 0);
    uint32_t v;
    intptr_t o = (intptr_t)*(uint32_t*)(ebx + 0x2c);
    if (t == 2 || (3 < t && t < 6)) v = *(uint32_t*)(o + 0xc);
    else v = o == 0 ? 0 : (uint32_t)o10867(o);
    esi[7] = v;
    if (t == 2 || (3 < t && t < 6)) v = *(uint32_t*)(o + 0x10);
    else v = o == 0 ? 0 : (uint32_t)o10750(o);
    esi[8] = v;
    esi[1] = (uint32_t)(ebx + 0x20);
    *(uint16_t*)(esi + 10) = 1;
    esi[0xc] = 2;
    esi[0x12] = *(uint32_t*)(ebx + 0x28);
    uint32_t w = (uint32_t)o10193(ebx);
    esi[0x10] = 2;
    esi[0x13] = w;
    return w;   // retail returns the last callee's EAX
}
