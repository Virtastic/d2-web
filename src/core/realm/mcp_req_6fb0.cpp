#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2MCPClient+0x6FB0: compose MCP request 0x0C = { u8 0x0C, strz }.
// Returns the byte count sent (1 + strlen + 1), or 0 where retail logs and _exit(-1)s (strlen+1 >= 0x400).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_6fb0(const char* s, uint8_t* out) {
    uint32_t n = (uint32_t)strlen(s) + 1;
    if (n >= 0x400) return 0;
    out[0] = 0x0C;
    memcpy(out + 1, s, n);
    return n + 1;
}
