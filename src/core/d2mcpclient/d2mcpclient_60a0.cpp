// D2MCPClient 0x60A0: the realm packet 0x00 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x60A0 (6 B): handler(ECX = packet, EDX = size).
// Returns 1; nothing is read or written.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in00_60a0(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    return 1;
}
