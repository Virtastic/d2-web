// 1.13c D2Client+0x43920 -- the client's net message pump (one call per main-loop iteration, from +0x44B50).
#pragma once
#include <cstdint>

// The callees, in the conventions retail passes them. The live table (d2net_local_drain) binds the local
// connection's takes and the client's dispatch; the FUNCTION probe binds the capture's hooks.
struct D2c43920Env {
    uint32_t (*take_system)(uint8_t* buf, uint32_t len);   // D2Net #10020 (+0x71E0) through thunk +0xD210: size, or -1 when empty
    uint32_t (*take_game)(uint8_t* buf, uint32_t len);     // D2Net #10008 (+0x7200) through thunk +0xD24C
    void (*acd70)(const uint8_t* packet);                  // +0xACD70 (ESI = packet): system 0xB3, a fragment
    void (*acbf0)(const uint8_t* packet);                  // +0xACBF0 (EAX = packet): system 0xB4
    void (*ace20)(const uint8_t* packet, int32_t size);    // +0xACE20 (EAX = packet, one stack word): the game-packet dispatch
    int32_t* flag;                                         // DAT_6FBCC39C (D2Client+0x11C39C): 0xAF sets it, 0xB0 clears it
};

extern "C" int32_t g_d2c_11c39c;   // D2Client+0x11C39C in the shipped module
extern "C" void d2c_43920(const D2c43920Env* e);
