// D2MCPClient: what the entries of the realm receive dispatch table share. Every entry is handler(ECX = packet, EDX = size),
// where the packet starts at its id byte, and returns 1 when the packet was taken, 0 when it was refused. The module state the
// handlers write is kept as the image range it occupies (0xAE80..0xB240), addressed by RVA, because several handlers copy
// strings across neighbouring fields; the character-list reassembly buffer follows at 0xB240. A handler reaches the rest of the
// module, and the callbacks the game registered in the slots 0xB224..0xB234, through McpEnv::call.
#pragma once
#include <cstdint>
#include <cstring>
#include <initializer_list>

struct McpRegs { uintptr_t eax = 0, ecx = 0, edx = 0, ebx = 0; };
struct McpEnv {
    // entry: a module entry point, or the RVA of a callback slot for the callback registered there
    uint32_t (*call)(uint32_t entry, const McpRegs& regs, const uintptr_t* stack, uint32_t nstack);
};
inline uint32_t mcpCall(const McpEnv* env, uint32_t entry, const McpRegs& regs, std::initializer_list<uintptr_t> stack = {}) {
    return env->call(entry, regs, stack.begin(), (uint32_t)stack.size());
}

enum : uint32_t {
    kMcpStateBase = 0xAE80, kMcpStateEnd = 0xB240,
    kMcpMotdReady = 0xAE80,        // message-of-the-day answer arrived
    kMcpDeleteReady = 0xAE84,
    kMcpMotdText = 0xAE88,
    kMcpJoinReady = 0xAEA4,
    kMcpAnswerId = 0xAEA8,         // word: request id of the game answer
    kMcpCreateReady = 0xAEAC,
    kMcpGameToken = 0xAEB0,
    kMcpRequestId = 0xAEB4,        // word: id of the request in flight
    kMcpGameAddress = 0xAED8,
    kMcpLogonReady = 0xB02C,
    kMcpListMore = 0xB030,
    kMcpInfo = 0xB034,             // game info / game list block, 0xB034..0xB1BC
    kMcpJoinStatus = 0xB1BC,
    kMcpMotdTitle = 0xB1C0,
    kMcpCancelReady = 0xB1C4,
    kMcpStatus = 0xB1C8,           // result of the last answer
    kMcpCharCreateReady = 0xB1CC,
    kMcpLadderReady = 0xB1D4,
    kMcpErrorText = 0xB1F8,
    kMcpStartupReady = 0xB20C,
    kMcpCharLogonReady = 0xB214,
    kMcpEvent = 0xB218,            // event handle the startup wait blocks on
    kMcpConvertReady = 0xB21C,
    kMcpOnGameList = 0xB224, kMcpOnGameInfo = 0xB228, kMcpOnLadder = 0xB22C, kMcpOnLadderStart = 0xB230, kMcpOnChars = 0xB234,
    kMcpLadderTotal = 0xB238, kMcpLadderHave = 0xB23C, kMcpLadderData = 0xB240,
};

extern "C" {
extern uint8_t g_mcp_6fa2ae80[kMcpStateEnd - kMcpStateBase];
extern uint8_t g_mcp_6fa2b240[0x10040];   // a chunk lands at a 16-bit offset with a 16-bit length under a 16-bit total
}
inline uint8_t* mcpAt(uint32_t rva) { return g_mcp_6fa2ae80 + (rva - kMcpStateBase); }
inline uint32_t mcpGet32(uint32_t rva) { uint32_t v; memcpy(&v, mcpAt(rva), 4); return v; }
inline uint16_t mcpGet16(uint32_t rva) { uint16_t v; memcpy(&v, mcpAt(rva), 2); return v; }
inline void mcpSet32(uint32_t rva, uint32_t v) { memcpy(mcpAt(rva), &v, 4); }
inline void mcpSet16(uint32_t rva, uint16_t v) { memcpy(mcpAt(rva), &v, 2); }
inline uint32_t mcpPkt32(const uint8_t* p, uint32_t off) { uint32_t v; memcpy(&v, p + off, 4); return v; }
inline uint16_t mcpPkt16(const uint8_t* p, uint32_t off) { uint16_t v; memcpy(&v, p + off, 2); return v; }
// unbounded string copy into the state, as retail does it; returns the byte after the source's terminator
inline const uint8_t* mcpStrCopy(uint32_t rva, const uint8_t* src) {
    uint8_t* d = mcpAt(rva);
    uint8_t* const end = g_mcp_6fa2ae80 + sizeof g_mcp_6fa2ae80;
    for (;;) { const uint8_t c = *src++; if (d < end) *d++ = c; if (!c) return src; }
}
