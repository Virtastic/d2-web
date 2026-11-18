// D2MCPClient 0x6540: the realm packet 0x03 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x6540 (41 B): handler(ECX = packet, EDX = size).
// When the word at packet+1 is the id of the request in flight (word 0xAEB4): stores the dword at packet+7 in the status word (0xB1C8), the id in the answer-id word (0xAEA8) and the dword at packet+3 in 0xAED8. Returns 1 either way.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in03_6540(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    if (mcpPkt16(packet, 1) == mcpGet16(kMcpRequestId)) {
        mcpSet32(kMcpStatus, mcpPkt32(packet, 7));
        mcpSet16(kMcpAnswerId, mcpGet16(kMcpRequestId));
        mcpSet32(kMcpGameAddress, mcpPkt32(packet, 3));
    }
    return 1;
}
