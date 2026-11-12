// D2MCPClient 0x60D0: the realm packet 0x14 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x60D0 (19 B): handler(ECX = packet, EDX = size).
// Stores the dword at packet+1 in 0xB1BC and sets the answer flag 0xAEA4 to 1. Returns 1. The size is not read.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in14_60d0(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    mcpSet32(kMcpJoinStatus, mcpPkt32(packet, 1));
    mcpSet32(kMcpJoinReady, 1);
    return 1;
}
