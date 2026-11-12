// D2MCPClient 0x5F10: the realm packet 0x11 entry of the receive dispatch table.
#include <emscripten.h>
#include "d2mcpclient_in.h"

// 1.13c D2MCPClient+0x5F10 (333 B): handler(ECX = packet, EDX = size).
// A packet under 8 bytes is refused (0). Fields: kind byte at +1, total at +2, chunk length at +4, chunk offset at +6 (words), data from +8. All three words zero: the list callback (slot 0xB22C) is called with zeros in ECX, EDX and three stack words, without a null test, the total (0xB238) and the received count (0xB23C) are cleared, returns 1. A total under 12 is refused. Otherwise the total is stored, the chunk is refused (0, with the received count left at the offset) when offset + length exceeds the total, else copied to the buffer 0xB240 at the offset and the received count becomes offset + length. While that is below the total the handler returns 1. Once complete: the start callback (slot 0xB230) runs if registered, then the buffer is walked as records of three dwords (first, entries, width) followed by entries * (width + 12) bytes: the remaining count drops by 12 and is stored in 0xB238; the walk stops on entries < 0, width < 0, entries > 0x100, width > 0x10, record bytes above the remaining count, or first > 2000; otherwise the list callback, if registered, gets first (ECX), entries (EDX) and the stack words width, kind, record data address; the remaining count then drops by the low 16 bits of the record bytes and the walk continues while it is positive. 0xB238 is cleared at the end. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_in11_5f10(const uint8_t* packet, uint32_t size, const McpEnv* env) {
    (void)packet; (void)size; (void)env;
    if (size < 8) return 0;
    const uint32_t kind = packet[1];
    const uint32_t total = mcpPkt16(packet, 2), len = mcpPkt16(packet, 4), offset = mcpPkt16(packet, 6);
    if (total == 0 && len == 0 && offset == 0) {   // an empty list: the callback is not tested here
        mcpCall(env, kMcpOnLadder, {}, { 0, 0, 0 });
        mcpSet32(kMcpLadderTotal, 0);
        mcpSet32(kMcpLadderHave, 0);
        return 1;
    }
    if (total < 0xC) return 0;
    mcpSet32(kMcpLadderTotal, total);
    mcpSet32(kMcpLadderHave, offset);
    const uint32_t have = len + offset;
    if (have > total) return 0;
    memcpy(g_mcp_6fa2b240 + offset, packet + 8, len);
    mcpSet32(kMcpLadderHave, have);
    if (have < total) return 1;
    if (mcpGet32(kMcpOnLadderStart) != 0) mcpCall(env, kMcpOnLadderStart, {});
    int32_t left = (int32_t)mcpGet32(kMcpLadderTotal);
    uint32_t pos = 0;
    do {
        if (pos + 12 > sizeof g_mcp_6fa2b240) break;
        int32_t head[3];
        memcpy(head, g_mcp_6fa2b240 + pos, 12);
        pos += 12;
        const int32_t bytes = (int32_t)((uint32_t)(head[2] + 0xC) * (uint32_t)head[1]);
        left -= 0xC;
        mcpSet32(kMcpLadderTotal, (uint32_t)left);
        if (head[1] < 0 || head[2] < 0 || head[1] > 0x100 || head[2] > 0x10 || bytes > left || head[0] > 2000) break;
        if (mcpGet32(kMcpOnLadder) != 0) {
            mcpCall(env, kMcpOnLadder, { 0, (uintptr_t)(uint32_t)head[0], (uintptr_t)(uint32_t)head[1] }, { (uintptr_t)(uint32_t)head[2], kind, (uintptr_t)(g_mcp_6fa2b240 + pos) });
            left = (int32_t)mcpGet32(kMcpLadderTotal);
        }
        left -= (int32_t)((uint32_t)bytes & 0xFFFF);
        pos += (uint32_t)bytes;
    } while (left > 0);
    mcpSet32(kMcpLadderTotal, 0);
    return 1;
}
