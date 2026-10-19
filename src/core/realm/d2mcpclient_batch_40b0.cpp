// D2MCPClient 1.13c CRT strcat. Retail scans dst, then copies src through its NUL.
// Its aligned dword loops are equivalent to bytewise operations on valid strings.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2MCPClient+0x040B0 (232 B, callers 0x025EC and 0x05603).
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t* d2_mcp_strcat_40b0(uint8_t* dst, const uint8_t* src) {
    uint8_t* const ret = dst;
    while (*dst != 0) ++dst;
    do {
        *dst++ = *src;
    } while (*src++ != 0);
    return ret;
}
