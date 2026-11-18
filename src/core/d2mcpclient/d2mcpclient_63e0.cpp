// D2MCPClient 0x63E0: the realm packet 0x05 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x63E0 (230 B): handler(ECX = packet, EDX = size).
// A packet whose word at packet+1 is not the id of the request in flight (word 0xAEB4) is taken and ignored. Otherwise the game-list block is filled: id word to 0xB034, dword packet+3 to 0xB038, byte packet+7 to 0xB044, dword packet+8 to 0xB048. A token of -2 sets the status word (0xB1C8) to 0x33, -1 to 0x32, and both stop there. Otherwise the string at packet+0xC goes to 0xB04C and the one after it to 0xB05C, and 0xB030 is 1 when bytes remain after the second string (its end offset is below the size), else 0. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in05_63e0(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    const uint32_t token = mcpPkt32(packet, 8);
    if (mcpPkt16(packet, 1) != mcpGet16(kMcpRequestId)) return 1;
    mcpSet16(0xB034, mcpGet16(kMcpRequestId));
    mcpSet32(0xB038, mcpPkt32(packet, 3));
    mcpSet32(0xB044, packet[7]);
    mcpSet32(0xB048, token);
    if (token == 0xFFFFFFFEu) { mcpSet32(kMcpStatus, 0x33); return 1; }
    if (token == 0xFFFFFFFFu) { mcpSet32(kMcpStatus, 0x32); return 1; }
    const uint8_t* desc = packet + 0xC + strlen((const char*)packet + 0xC) + 1;
    const uint8_t* end = desc + strlen((const char*)desc) + 1;
    mcpStrCopy(0xB04C, packet + 0xC);
    mcpStrCopy(0xB05C, desc);
    mcpSet32(kMcpListMore, (uint32_t)(end - packet) < size);
    return 1;
}
