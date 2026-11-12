// D2MCPClient 0x60C0: the realm packet 0x17 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x60C0 (13 B): handler(ECX = packet, EDX = size).
// Hands the packet and its size (two stack words) to 0x65D0, which keeps a copy of the packet, and returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in17_60c0(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    mcpCall(env, 0x65D0, {}, { (uintptr_t)packet, size });
    return 1;
}
