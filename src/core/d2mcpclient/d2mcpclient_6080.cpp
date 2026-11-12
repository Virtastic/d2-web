// D2MCPClient 0x6080: the realm packet 0x08 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x6080 (24 B): handler(ECX = packet, EDX = size).
// When a callback is registered in the slot, calls it with the word at packet+1 (CX) and the address packet+3 (EDX): the game-list entry callback. Returns 1 either way. The size is not read.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in08_6080(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    if (mcpGet32(kMcpOnGameList) != 0) mcpCall(env, kMcpOnGameList, { 0, mcpPkt16(packet, 1), (uintptr_t)(packet + 3) });
    return 1;
}
