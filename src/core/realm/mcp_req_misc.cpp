#include <cstdint>
#include <cstring>
#include <emscripten.h>

// MCP request composers sharing sub_6EC0 (append strz to running length; _exit(-1) once length >= 0x400).
// Each returns the byte count sent, or 0 where retail would _exit. 
// Append strz at out+off with running length *len; false = retail's overflow exit.
static bool app(const char* s, uint32_t* len, uint8_t* at) {
    uint32_t n = (uint32_t)strlen(s) + 1;
    *len += n;
    if (*len >= 0x400) return false;
    memcpy(at, s, n);
    return true;
}

// 1.13c D2MCPClient+0x6F20: { 0x16, u32 a, u32 b, strz }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_6f20(const char* s, uint32_t a, uint32_t b, uint8_t* out) {
    uint32_t len = 0;
    out[0] = 0x16; memcpy(out + 1, &a, 4); memcpy(out + 5, &b, 4);
    return app(s, &len, out + 9) ? len + 9 : 0;
}
// 1.13c D2MCPClient+0x7020: { 0x0B, strz, strz }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_7020(const char* s1, const char* s2, uint8_t* out) {
    uint32_t len = 0;
    out[0] = 0x0B;
    if (!app(s1, &len, out + 1)) return 0;
    uint32_t at = len + 1;   // (an argument expression reading len would race app's update of it)
    return app(s2, &len, out + at) ? len + 1 : 0;
}
// 1.13c D2MCPClient+0x70B0 (op 9) and +0x71D0 (op 6): { op, u16 word, strz }; both store word to [0xaeb4].
static uint32_t op_word_str(uint8_t op, const char* s, uint16_t w, uint8_t* out) {
    uint32_t len = 0;
    out[0] = op; out[1] = w & 0xFF; out[2] = w >> 8;
    return app(s, &len, out + 3) ? len + 3 : 0;
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_70b0(const char* s, uint16_t w, uint8_t* out) { return op_word_str(9, s, w, out); }
// ponytail: 71D0 has no 0xFFFF check; unreachable after the 0x400 bound, so identical.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_71d0(const char* s, uint16_t w, uint8_t* out) { return op_word_str(6, s, w, out); }
