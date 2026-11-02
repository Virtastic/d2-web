// D2Client+0xAEB10 env seam, shared by the port and its browser wrapper (stat_update_aeb10.cpp).
#pragma once
#include <cstdint>
struct D2cAEB10Env {
    void (*f32FE0)(void* unit);   // D2Client+0x82FE0
    void (*f2A6C0)();             // D2Client+0x2A6C0 (ESI)
    void (*fA6210)();             // D2Client+0xA6210 (ESI)
    void (*fAE3F0)(void* unit);   // D2Client+0xAE3F0 (EAX=unit)
    void (*fAC970)();             // D2Client+0xAC970 (EBX)
    void* curUnit;                // global DAT 0xCBBFC (6fbcbbfc)
};
extern "C" void d2c_aeb10(const D2cAEB10Env* e, void* self, int32_t ev, int32_t p1);
