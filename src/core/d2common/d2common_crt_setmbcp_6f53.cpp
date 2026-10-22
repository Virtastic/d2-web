// D2Common 0x06F53: MSVC CRT _setmbcp (code-page selector). Same VC7.1 routine as
// D2MCPClient+0x031FA; the realm port is reused with the export renamed. Its five code-page records
// (932/936/949/950/951-class, stride 0x30 at RVA 0x...9978) are taken as identical to that port's.
// Callees 0x6D60/0x6C0C/0x6DB8 are the host-supplied derive step (env->derive); setSBCS is inline.
#define d2_mcp_setmbcp_31fa d2common_setmbcp_6f53
#include "../realm/mcp_setmbcp_31fa.cpp"

// 1.13c D2Common+0x06F53
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2common_setmbcp_6f53_entry(
    uint32_t cp, uint32_t pseudo, D2McpMbcpState* state, const D2McpMbcpEnv* env) {
    return d2common_setmbcp_6f53(cp, pseudo, state, env);
}
