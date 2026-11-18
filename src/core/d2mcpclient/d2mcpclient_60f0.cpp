// D2MCPClient 0x60F0: the realm packet 0x10 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x60F0 (98 B): handler(ECX = packet, EDX = size).
// Stores the dword at packet+1 in 0xAED8, the dword at packet+5 in 0xAEB0 and the dword at packet+0xD in the status word (0xB1C8). When the dword at packet+9 is nonzero the localised string 0x12 (0x5E16) is copied to the error text 0xB1F8 (0x5DDA); otherwise a status of 0 becomes 0x28. The answer flag 0xAEAC is set to 1 in both cases. Returns 1. The size is not read.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in10_60f0(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    const uint32_t result = mcpPkt32(packet, 0xD);
    mcpSet32(kMcpGameAddress, mcpPkt32(packet, 1));
    mcpSet32(kMcpStatus, result);
    mcpSet32(kMcpGameToken, mcpPkt32(packet, 5));
    if (mcpPkt32(packet, 9) != 0) {
        const uint32_t text = mcpCall(env, 0x5E16, {}, { 0x12 });
        mcpCall(env, 0x5DDA, {}, { (uintptr_t)mcpAt(kMcpErrorText), text });
        mcpSet32(kMcpCreateReady, 1);
        return 1;
    }
    mcpSet32(kMcpCreateReady, 1);
    if (result == 0) mcpSet32(kMcpStatus, 0x28);
    return 1;
}
