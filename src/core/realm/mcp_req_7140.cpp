#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2MCPClient+0x7140: compose MCP request 0x08 = { u8 8, u16 word, strz }.
// Returns the byte count sent (3 + strlen + 1), or 0 where retail logs and _exit(-1)s (strlen+1 >= 0x400).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_7140(const char* s, uint16_t word, uint8_t* out) {
    uint32_t n = (uint32_t)strlen(s) + 1;
    if (n >= 0x400) return 0;
    out[0] = 8; out[1] = word & 0xFF; out[2] = word >> 8;
    memcpy(out + 3, s, n);
    return n + 3;
}
