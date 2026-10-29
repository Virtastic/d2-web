// Session-replay driver for the D2Client per-frame pump: runs 1.13c D2Client+0x44B50 (mode 2, game screen) once per
// tick through a host that routes its 0x44A80 callee to our port and answers every other callee 0 (state-free, so
// no compared record changes). The play client's own frame is play/gameplay-shell.mjs render() -> d2_client_frame_g.
#include <emscripten.h>
#include <cstdint>
#include "client_host.h"
extern "C" uint32_t d2_client_44b50(D2cHost, void*, const uint32_t*);
extern "C" uint32_t d2_client_44a80(D2cHost, uint32_t*, const uint32_t*);
extern "C" void d2c_423a0(uint32_t, uint32_t*, int32_t*, int32_t*, int32_t*, int32_t*);
uint32_t D2Client_Tier42400(int32_t, int32_t);
namespace {
uint32_t g44b50[40], g44a80[6], now[3];
// g44b50 indices: 6 t97EC, 11 t9818, 12 v9808, 13 v9810, 14 v980C, 15 v9814 (D2c44B50G order)
uint32_t host(D2cCall* c) {
  uint32_t* g = g44b50;
  switch (c->rva) {
    case 0x44A80: return d2_client_44a80(host, g44a80, now);
    case 0x423A0: d2c_423a0(g[6], &g[11], (int32_t*)&g[12], (int32_t*)&g[14], (int32_t*)&g[13], (int32_t*)&g[15]); return 0;
    case 0x42400: return D2Client_Tier42400((int32_t)c->ecx, (int32_t)c->edx);
  }
  return 0;
}
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_session_frame(uint32_t tick_ms) {
  now[0] = now[1] = now[2] = tick_ms;
  g44b50[0] = 2;                      // C394 mode: not the 0/1 front-end paths
  g44b50[1] = 40;                     // BA4318 frameMs
  g44b50[19] = 1;                     // c3a0 (CC3A0): game running
  return d2_client_44b50(host, g44b50, now);
}
