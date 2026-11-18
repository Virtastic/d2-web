// D2MCPClient 0x5EE0: the realm packet 0x12 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x5EE0 (38 B): handler(ECX = packet, EDX = size).
// A packet under 2 bytes is refused (0). When a callback is registered in the slot 0xB234 it is called with the byte at packet+1 (ECX), the size less 2 (EDX) and the address packet+2 (one stack word). Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in12_5ee0(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    if (size < 2) return 0;
    if (mcpGet32(kMcpOnChars) != 0) mcpCall(env, kMcpOnChars, { 0, packet[1], size - 2 }, { (uintptr_t)(packet + 2) });
    return 1;
}
