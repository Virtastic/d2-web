// D2MCPClient 0x61A0: the realm packet 0x0E entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x61A0 (73 B): handler(ECX = packet, EDX = size).
// The packet carries two strings from packet+1. The first is copied to 0xB1C0 and the second to 0xAE88, both unbounded, the first before the second; then the answer flag 0xAE80 is set to 1. Returns 1. The size is not read.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in0e_61a0(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    const uint8_t* text = mcpStrCopy(kMcpMotdTitle, packet + 1);
    mcpStrCopy(kMcpMotdText, text);
    mcpSet32(kMcpMotdReady, 1);
    return 1;
}
