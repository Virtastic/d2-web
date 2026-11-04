// src/core/fog/fog_sub_19530.cpp -- 1.13c Fog+0x19530 (log-subsystem init).
// The log-manager globals live in fog_batch_b144.cpp (fogb144pub); this init writes them in retail's order.
#include <cstdint>
#include <Windows.h>

namespace fogb144pub {
extern HANDLE g_thread;                                   // 0x6FF818A0 writer thread
extern uint32_t g_stop, g_818f8, g_819a4;                 // 0x6FF819A0, 0x6FF818F8, 0x6FF819A4 (thread id)
extern SYSTEMTIME g_startup;                              // 0x6FF81838
extern uint32_t g_banner[20];                             // 0x6FF81850..0x6FF8189C
}

// Win32 seams. The writer thread proc is retail's Fog+0x193E0, passed as its retail address.
struct FogLogInitApi {
    HANDLE (*create_thread)(uint32_t sa, uint32_t stack, uint32_t proc, uint32_t param, uint32_t flags, uint32_t* id);
    void (*local_time)(SYSTEMTIME* out);
};
static HANDLE noThread(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t*) { return nullptr; }   // wasm: no writer thread
static void localTime(SYSTEMTIME* out) { GetLocalTime(out); }
static const FogLogInitApi kFogLogInitApi = { noThread, localTime };
const FogLogInitApi* g_fog_log_init_api = &kFogLogInitApi;

// 1.13c Fog+0x19530
// Clear the stop flag, start the writer thread (CreateThread(0, 0, 0x6FF693E0, 0, 0, &id 0x6FF819A4) into 0x6FF818A0),
// set the running flag 0x6FF818F8, take the startup time (GetLocalTime into 0x6FF81838), arm all 20 startup banners.
void fog_sub_19530() {
    using namespace fogb144pub;
    const FogLogInitApi* api = g_fog_log_init_api;
    g_stop = 0;
    g_thread = api->create_thread(0, 0, 0x6FF693E0u, 0, 0, &g_819a4);
    g_818f8 = 1;
    api->local_time(&g_startup);
    for (uint32_t& e : g_banner) e = 1;
}
