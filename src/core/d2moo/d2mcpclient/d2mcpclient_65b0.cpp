// D2MCPClient 0x65B0: the realm packet 0x01 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x65B0 (27 B): handler(ECX = packet, EDX = size).
// Stores the dword at packet+1 in the status word (0xB1C8) and signals the event whose handle is in 0xB218 (SetEvent, import slot 0x8120). Returns 1. The size is not read.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in01_65b0(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    mcpSet32(kMcpStatus, mcpPkt32(packet, 1));
    mcpCall(env, 0x8120, {}, { mcpGet32(kMcpEvent) });
    return 1;
}
