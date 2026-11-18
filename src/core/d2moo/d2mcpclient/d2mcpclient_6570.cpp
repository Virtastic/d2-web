// D2MCPClient 0x6570: the realm packet 0x07 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x6570 (19 B): handler(ECX = packet, EDX = size).
// Stores the result dword at packet+1 in the status word (0xB1C8) and sets the answer flag of request 0x07 (0xB20C) to 1. Returns 1. The size is not read.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in07_6570(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    mcpSet32(kMcpStatus, mcpPkt32(packet, 1));
    mcpSet32(kMcpStartupReady, 1);
    return 1;
}
