// src/core/fog/fog_sub_19530.cpp -- 1.13c Fog+0x19530 (log-subsystem init).
#include <cstdint>

extern "C" { uint32_t g_fog_819a0; uint32_t g_fog_818f8; }        // DAT_6ff819a0 shutting-down flag
static uint32_t g_fog_81850[20];    // DAT_6ff81850..8189c, per-channel enables (1 each)

// 1.13c Fog+0x19530
// ponytail: CreateThread(0x693e0 log writer) and GetLocalTime(0x81838) omitted: no threads or wall clock in wasm.
void fog_sub_19530() {
    g_fog_819a0 = 0;
    g_fog_818f8 = 1;
    for (uint32_t& e : g_fog_81850) e = 1;
}
