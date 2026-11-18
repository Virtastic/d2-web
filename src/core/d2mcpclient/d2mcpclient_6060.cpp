// D2MCPClient 0x6060: the realm packet 0x09 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x6060 (24 B): handler(ECX = packet, EDX = size).
// When a callback is registered in the slot, calls it with the word at packet+1 (CX) and the address packet+4 (EDX): the game-info callback. Returns 1 either way. The size is not read.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in09_6060(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    if (mcpGet32(kMcpOnGameInfo) != 0) mcpCall(env, kMcpOnGameInfo, { 0, mcpPkt16(packet, 1), (uintptr_t)(packet + 4) });
    return 1;
}
