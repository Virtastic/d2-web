#include <cstdint>
#include <emscripten.h>

// 1.13c D2MCPClient+0x69F0 (ordinal 10054): the socket liveness probe. It reads the module global [0xb028] (the FSafeSock
// at the head of the MCP client state) and asks Fog#10079 for the socket object behind it -- that call returns 0 for a null
// pointer, so a null [0xb028] and a not-yet-connected socket object are the same answer: 0, the function returns without
// touching Fog#10012. Otherwise it fills a 16-byte sockaddr_in on its stack, sets the stack length word to 0x10 and calls
// Fog#10012(obj, &sockaddr, &len), Fog's SOCKET_GetPeerName (ws2_32 getpeername on the socket inside the object); the
// address it writes is discarded, only that call's BOOL is returned. `obj` and `peerOk` are those two callee results, which
// the caller (the FN probe, or the realm side that owns the Fog imports) supplies. Nothing is written to any global.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_is_connected_69f0(uint32_t obj, uint32_t peerOk) {
    return obj ? peerOk : 0;
}
