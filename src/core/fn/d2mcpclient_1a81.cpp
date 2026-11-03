// 1.13c D2MCPClient+0x1A81 sub_1A81: the __unlock(0xd) thunk, probed through d2_mcp_unlock_d's function-pointer seam.
// r[0] = hooklog blob (@lg). Retail's _unlock (0x14EE) is hooked; the probe records the call with the same layout.
#include <emscripten.h>
#include <cstdint>
extern "C" void d2_mcp_unlock_d(void (*unlock)(int32_t));
static uint32_t* g_lg;
static void rec(int32_t n) { uint32_t* w = g_lg + 1 + 10 * g_lg[0]++; for (int i = 0; i < 10; i++) w[i] = 0; w[0] = 0x14EE; w[6] = (uint32_t)n; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2mcpclient_1a81(uint32_t* r) {
  g_lg = (uint32_t*)(uintptr_t)r[0];
  for (int i = 0; i < 1 + 10 * 8; i++) g_lg[i] = 0;
  d2_mcp_unlock_d(rec);
  r[4] = 0;
}
