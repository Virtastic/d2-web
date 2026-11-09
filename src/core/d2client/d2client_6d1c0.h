// Seams of D2Client 0x6D1C0 (the load-complete pump: the all-rooms walk 0x6D010 run under the loading-bar gate).
#pragma once
#include "d2client_6d010.h"
struct D2Client6d1c0Env {
    D2Client6d010Env walk;                 // the 0x6D010 seams (g04c reads the gate dword D2Client+0x11C04C)
    void     (*setGate)(uint32_t v);       // dword D2Client+0x11C04C
    uint32_t (*getGate)();
    uint32_t (*tick)();                    // GetTickCount (IAT D2Client+0xB7EF5C)
    uint32_t (*getLast)();                 // dword D2Client+0x11C03C (last pump tick)
    void     (*setLast)(uint32_t v);
};
// 1.13c D2Client+0x6D1C0: no arguments, plain ret.
void d2c_6d1c0(const D2Client6d1c0Env& e);
