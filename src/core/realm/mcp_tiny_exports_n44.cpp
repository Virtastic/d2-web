#include <cstdint>
#include <cstring>
#include <emscripten.h>

// D2MCPClient exported thunks, setters and the CRT message-box helper. Host seams (socket send, user32) are arguments.
extern "C" {
uint32_t d2_mcp_req_7410(const char* s, uint8_t* out);
uint32_t d2_mcp_req_7480(const char* s, uint32_t cls, uint16_t flags, uint8_t* out);
}

static uint32_t g_b1c8;          // [0xb1c8]
static uint32_t g_b22c, g_b230;  // [0xb22c], [0xb230]
static uint32_t g_b030[99];      // [0xb030], 0x18c bytes
static uint16_t g_aea8;          // [0xaea8]

// 1.13c D2MCPClient+0x06AA0: thunk (12 B) into sub_7410 (CHARLOGON); returns the frame length, caller sends it.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_6aa0(const char* s, uint8_t* out) { return d2_mcp_req_7410(s, out); }

// 1.13c D2MCPClient+0x06AB0: thunk (22 B, ret 0xc) into sub_7480 (CHARCREATE).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_6ab0(const char* s, uint32_t cls, uint16_t flags, uint8_t* out) {
    return d2_mcp_req_7480(s, cls, flags, out);
}

// 1.13c D2MCPClient+0x06DB0 (ordinal 10003): zero [0xb1c8], send the single byte 0x0F; the frame is returned (length 1).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_6db0(uint8_t* out) { g_b1c8 = 0; out[0] = 0x0F; return 1; }

// 1.13c D2MCPClient+0x05E50 (ordinal 10046, fastcall): store the two register arguments at [0xb22c], [0xb230].
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_5e50(uint32_t a, uint32_t b) { g_b22c = a; g_b230 = b; }

// 1.13c D2MCPClient+0x067D0 (ordinal 10017): zero 99 dwords at [0xb030].
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_67d0() { memset(g_b030, 0, sizeof g_b030); }

// 1.13c D2MCPClient+0x06970 (ordinal 10027): return the word at [0xaea8].
extern "C" EMSCRIPTEN_KEEPALIVE uint16_t d2_mcp_6970() { return g_aea8; }

// 1.13c D2MCPClient+0x0456D ___crtMessageBoxA. user32 is the host: api is the resolved import set (null = absent),
// osver/winver are [0xa9e4]/[0xa9f0]. winsta_flags = GetUserObjectInformation flags byte, or -1 when the station
// lookup fails. Returns the MessageBoxA result.
struct CrtUser32 {
    int (*MessageBoxA)(uintptr_t owner, const char* text, const char* cap, uint32_t type);
    uintptr_t (*GetActiveWindow)();
    uintptr_t (*GetLastActivePopup)(uintptr_t);
    int (*GetProcessWindowStation)();   // null unless platform==2 and GetUserObjectInformationA resolved
    int (*GetUserObjectInformationA)(int station, int idx, void* buf, int len, int* need);
};
extern "C" EMSCRIPTEN_KEEPALIVE int d2_mcp_crt_msgbox_456d(const CrtUser32* u, uint32_t winver, const char* text, const char* cap, uint32_t type) {
    if (!u || !u->MessageBoxA) return 0;
    uintptr_t owner = 0;
    bool visible = true;
    if (u->GetProcessWindowStation) {
        int st = u->GetProcessWindowStation(), need;
        uint8_t info[12] = {};
        visible = st && u->GetUserObjectInformationA(st, 1, info, 12, &need) && (info[8] & 1);
    }
    if (visible) {
        if (u->GetActiveWindow && (owner = u->GetActiveWindow()) != 0 && u->GetLastActivePopup)
            owner = u->GetLastActivePopup(owner);
    } else type |= winver < 4 ? 0x40000 : 0x200000;
    return u->MessageBoxA(owner, text, cap, type);
}
