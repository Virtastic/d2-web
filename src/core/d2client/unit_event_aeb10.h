// D2Client+0xAEB10 env seam, shared by the port and its browser wrapper (stat_update_aeb10.cpp).
#pragma once
#include <cstdint>
struct D2cAEB10Env {
    void (*f32FE0)(void* unit);   // D2Client+0x82FE0
    void (*f2A6C0)(void* esi);      // D2Client+0x2A6C0 (ESI=unit, EDI=5)
    void (*fA6210)(void* esi);      // D2Client+0xA6210 (ESI=unit)
    void (*fAE3F0)(void* eax);     // D2Client+0xAE3F0 (EAX=the global current unit)
    void (*fAC970)(void* ebx);      // D2Client+0xAC970 (EBX=unit)
    void* curUnit;                // global DAT 0x11BBFC (6fbcbbfc)
};
extern "C" void d2c_aeb10(const D2cAEB10Env* e, void* self, int32_t ev, int32_t p1);
