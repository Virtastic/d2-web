// D2MCPClient 0x6280: the realm packet 0x06 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x6280 (337 B): handler(ECX = packet, EDX = size).
// A packet whose word at packet+1 is not the id of the request in flight (word 0xAEB4) is taken and ignored. Otherwise the game-info block is filled: id word to 0xB034, dword packet+3 to 0xB048, the player count byte packet+0xE to 0xB044, dword packet+7 to 0xB03C, bytes packet+0xB..0xD to 0xB040..0xB042. A token of -1 sets the status word (0xB1C8) to 0x32, -2 to 0x33, and both stop there. Otherwise the string at packet+0x2F goes to 0xB05C, and for each player i under the count: byte packet+0xF+i widened to the word 0xB07C+2i, byte packet+0x1F+i widened to the word 0xB09C+2i, and the next string of the packet copied to 0xB0BC+0x10i. The count byte is read again from the packet after every player. Returns 1. The size is not read.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in06_6280(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    const uint32_t token = mcpPkt32(packet, 3);
    if (mcpPkt16(packet, 1) != mcpGet16(kMcpRequestId)) return 1;
    const uint32_t count = packet[0xE];
    mcpSet16(0xB034, mcpGet16(kMcpRequestId));
    mcpSet32(0xB048, token);
    mcpSet32(0xB044, count);
    mcpSet32(0xB03C, mcpPkt32(packet, 7));
    *mcpAt(0xB040) = packet[0xB];
    *mcpAt(0xB041) = packet[0xC];
    *mcpAt(0xB042) = packet[0xD];
    if (token == 0xFFFFFFFFu) { mcpSet32(kMcpStatus, 0x32); return 1; }
    if (token == 0xFFFFFFFEu) { mcpSet32(kMcpStatus, 0x33); return 1; }
    const uint8_t* name = mcpStrCopy(0xB05C, packet + 0x2F);
    if (count != 0) {
        uint32_t i = 0;
        do {
            mcpSet16(0xB07C + 2 * i, packet[0xF + i]);
            mcpSet16(0xB09C + 2 * i, packet[0x1F + i]);
            name = mcpStrCopy(0xB0BC + 0x10 * i, name);
            ++i;
        } while ((int32_t)i < (int32_t)packet[0xE]);
    }
    return 1;
}
