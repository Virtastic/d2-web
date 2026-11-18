// D2MCPClient 0x6180: the realm packet 0x0D entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x6180 (19 B): handler(ECX = packet, EDX = size).
// Stores the result dword at packet+1 in the status word (0xB1C8) and sets the answer flag of request 0x0D (0xB1D4) to 1. Returns 1. The size is not read.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in0d_6180(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    mcpSet32(kMcpStatus, mcpPkt32(packet, 1));
    mcpSet32(kMcpLadderReady, 1);
    return 1;
}
