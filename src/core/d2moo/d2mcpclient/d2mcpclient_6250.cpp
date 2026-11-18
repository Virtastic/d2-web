// D2MCPClient 0x6250: the realm packet 0x0A entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x6250 (33 B): handler(ECX = packet, EDX = size).
// When the word at packet+1 is the id of the request in flight (word 0xAEB4), stores the dword at packet+3 in the status word (0xB1C8) and sets the answer flag 0xB1C4 to 1. Returns 1 either way.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in0a_6250(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    if (mcpPkt16(packet, 1) == mcpGet16(kMcpRequestId)) {
        mcpSet32(kMcpStatus, mcpPkt32(packet, 3));
        mcpSet32(kMcpCancelReady, 1);
    }
    return 1;
}
