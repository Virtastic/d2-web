// D2Client 0x40DF0 (random-offset effect burst) with its three non-seed callees as an environment, so the
// FN probe can record them (src/core/fn/d2client_40df0.cpp) and the engine wrapper can bind the real ones.
#pragma once
#include <cstdint>
struct D2c40df0Env {
    void* (*create)(const uint32_t* req23);   // D2Client+0xB30F0, ECX = 23-dword request, returns the missile or null
    void (*setX)(void* m, int32_t v);         // D2Common #10637 MISSILE_SetTargetX
    void (*setY)(void* m, int32_t v);         // D2Common #10019 MISSILE_SetTargetY
};
// EAX = n, EDX = seedLow (what #10614 receives), six stack arguments p1..p6 (ret 0x18).
extern "C" void d2c_40df0(int32_t n, uint32_t seedLow, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4,
                          uint32_t p5, uint32_t p6, const D2c40df0Env* env);
