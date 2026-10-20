// 1.13c D2MCPClient+0x6AD0 probe: ours, with the platform/Fog boundary scripted as the capture's
// hooks were: `in` = {send ret, alive #1, alive #2, pump ret}; the stubs re-emit the hook log (hl), sent bytes (lg) and [0xb1c8] (gd).
// Args: r[0]=reply r[1]=gp r[2]=gd r[3]=hl r[5]=lg r[6]=in.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "core/realm/mcp_startup.h"
static uint32_t *g_hl, *g_lg; static const uint32_t* g_in; static uint32_t g_alive, g_obj;
static void logCall(const uint32_t (&e)[10]) { memcpy(g_hl + 1 + 10 * g_hl[0]++, e, sizeof e); }
extern "C" {
void* d2_mcp_os_create_event() { return (void*)1; }
uint32_t d2_mcp_os_tick_count() { return 0xC0DE1000u; }
uint32_t d2_mcp_os_wait_event(void*, uint32_t) { return 0x102; }
void d2_mcp_os_sleep(uint32_t) {}
int32_t d2_mcp_fog_socket_alive(void* s) {
    g_obj = g_in[1 + (g_alive < 1 ? g_alive : 1)]; ++g_alive;
    logCall({0x5DF2, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)s, 0, 0, 0}); return g_obj != 0;
}
int32_t d2_mcp_fog_pump(uint32_t*) { logCall({0x5E2E, 0, g_obj, 0xB3000000u, 0, 0, 0xB3000000u, 0, 0, 0}); return g_in[3] != 0; }
int32_t d2_mcp_fog_send(void* s, const uint8_t* b, uint32_t n) {
    logCall({0x5DE6, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)s, 0xB3000000u, n, 0});
    uint32_t* p = g_lg + 1; g_lg[0] = 1; p[0] = n; memcpy(p + 1, b, n); memset((char*)(p + 1) + n, 0, (4 - n % 4) % 4);
    return g_in[0];
}
[[noreturn]] void d2_mcp_startup_overflow(uint32_t) { __builtin_trap(); }
EMSCRIPTEN_KEEPALIVE void d2_fn_d2mcpclient_6ad0(uint32_t* r) {
    const uint32_t* gp = (const uint32_t*)(uintptr_t)r[1]; uint32_t* gd = (uint32_t*)(uintptr_t)r[2];
    g_hl = (uint32_t*)(uintptr_t)r[3]; g_lg = (uint32_t*)(uintptr_t)r[5]; g_in = (const uint32_t*)(uintptr_t)r[6]; g_alive = 0; g_hl[0] = 0;
    d2_mcp_startup_state = { (void*)(uintptr_t)gp[0], nullptr, gp[1] };
    d2_mcp_startup_wait((const uint8_t*)(uintptr_t)r[0]);
    gd[0] = d2_mcp_startup_state.result; r[4] = 0;
}
}
