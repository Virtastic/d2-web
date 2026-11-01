// D2Client+0x25040 / +0x259F0 shared environment: the leaf callees are host seams (the probe logs them as hook records).
#pragma once
#include <cstdint>
struct D2c25040Env {
    void (*c43c)(void* u, uint32_t v);                          // D2Common 11090 (u, v), ret 8
    void (*refresh)(void* u, uint32_t v);                       // 0x6A220: EAX = u, stack v
    uint32_t (*f1660)(void* u);                                 // thiscall ECX = u
    uint32_t (*f1630)(void* u);                                 // thiscall ECX = u
    uint32_t (*c1f0)(void* u, uint32_t a, uint32_t b);          // (u, r1630, r1660)
    void (*c67c)(void* u, uint32_t v);                          // (u, r_c1f0)
    void (*ca1e)(void* u);                                      // D2Common 10408
    void (*fatal)(uint32_t line);                               // Fog 10265 then exit
};
void d2_client_25040(uint8_t* u, int32_t mode, uint32_t p1, const D2c25040Env* e);
struct D2c259Env {
    D2c25040Env p;
    void (*f6b300)(void* u);                                    // EAX = 0x47, stack (u,3,0,0,0,0,0)
    void (*animate)(void* u, uint8_t ax, uint32_t a, uint32_t b, uint32_t c);   // 0x24920
    void (*advance)(void* u);                                   // 0x237E0
    void (*update)(void* u);                                    // D2Common 10595
};
extern "C" void d2_client_259f0(uint8_t* u, const uint32_t* eax, const D2c259Env* e);
