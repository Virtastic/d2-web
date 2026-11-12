// 1.13c D2Client+0x443D0 -- client game-session init: the globals it reaches, one
// pointer per contiguous retail range (retail layout inside each range), and the two handler values it stores.
#pragma once
#include <cstdint>
#include "client_host.h"

struct D2c443D0G {
    uint32_t* s37c;          // 0x11C37C..0x11C41B (40 dwords): [0] join clock, [6] state, [9] entered, [10] exit queued, [11] loading, [18] session ptr
    uint32_t* c570;          // 0x11C570: packet handler
    uint32_t* c2e0;          // 0x11C2E0..0x11C2EF
    uint8_t* c024;           // 0x11C024: u16 at +0, u32 at +4
    uint32_t* bbfc;          // 0x11BBFC: pool teardown flag
    uint32_t* b800;          // 0x11B800..0x11B81F
    uint32_t* p97e0;         // 0x1197E0..0x119887 (42 dwords): [1] frame callback, [2] pool handle, [41] loading clock
    uint32_t* p960c;         // 0x11960C..0x119637
    uint32_t* p3f5c;         // 0x113F5C
    uint32_t* p212c;         // 0x0F212C..0x0F2137
    uint32_t* p27d0;         // 0x0E27D0..0x0E27D7
    const uint32_t* aae20;   // 0x0FAE20: pre-join wait timed out
    const uint32_t* p9e14;   // 0x0F9E14 width, 0x0F9E18 height - 40
    uint32_t* p9a08;         // 0x109A08..0x10B207 (0x600 dwords)
    uint32_t handler;        // value stored at 0x11C570 (retail: module+0x42470)
    uint32_t frameCb;        // value stored at 0x1197E4 (retail: module+0x44230)
    uint32_t (*tick)();      // GetTickCount
};

extern "C" void d2_client_443d0(D2cHost h, const D2c443D0G* g);
