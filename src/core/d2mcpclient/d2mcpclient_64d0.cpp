// D2MCPClient 0x64D0: the realm packet 0x04 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x64D0 (104 B): handler(ECX = packet, EDX = size).
// A packet whose word at packet+1 is not the id of the request in flight (word 0xAEB4) is taken and ignored. Otherwise: the id goes to the answer-id word (0xAEA8), the dword at packet+3 to 0xAED8 and the dword at packet+0xB to 0xAEB0. When the dword at packet+7 is nonzero, the localised string 0x12 (0x5E16) is copied to the error text 0xB1F8 (0x5DDA) and the status word is left alone; otherwise the status word (0xB1C8) takes the dword at packet+0xF, or 0x28 when that is 0. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in04_64d0(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    if (mcpPkt16(packet, 1) != mcpGet16(kMcpRequestId)) return 1;
    mcpSet16(kMcpAnswerId, mcpGet16(kMcpRequestId));
    mcpSet32(kMcpGameAddress, mcpPkt32(packet, 3));
    mcpSet32(kMcpGameToken, mcpPkt32(packet, 0xB));
    if (mcpPkt32(packet, 7) != 0) {
        const uint32_t text = mcpCall(env, 0x5E16, {}, { 0x12 });
        mcpCall(env, 0x5DDA, {}, { (uintptr_t)mcpAt(kMcpErrorText), text });
        return 1;
    }
    const uint32_t result = mcpPkt32(packet, 0xF);
    mcpSet32(kMcpStatus, result != 0 ? result : 0x28);
    return 1;
}
