// The browser realm client's request composers, routed through the 1.13c ports of BNClient and D2MCPClient
// (the D2GS receive side sizes packets through D2Net+0x6750 in d2gs_client_packets.cpp). Every host hook the
// retail functions take is pinned here, so each export is a pure function of its arguments: the bytes are what
// the retail function would send on a host answering the pins below.
//
// Pins (the retail import each one stands in for):
//   GetTickCount -> the caller's client token        Storm SStrCopy -> copy at most cap-1 bytes, NUL, return length
//   CharLowerBuffA -> ASCII lowercase                 GetSystemTime/GetLocalTime -> the same instant (bias 0)
//   GetUserDefaultLCID / GetSystemDefaultUILanguage -> 0x409    GetLocaleInfoA -> "USA" / "United States"
//   D2Lang #10004 (product language) -> leaves the zero      no socket (local address 0)
// The frame sender (BNClient 0xEB00, the Fog MCP send) is replaced by a capture into `out`.
//
// BNCS exports return the frame BODY length (the caller frames it with d2_realm_bncs_header); MCP exports return
// the body after the op byte (framed with d2_realm_mcp_header). 0 is returned where retail would not send.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
uint8_t* g_out = nullptr;
uint32_t g_outLen = 0, g_tick = 0;
uint32_t sstrcopy(char* d, const char* s, uint32_t cap) { uint32_t n = 0; while (n + 1 < cap && s[n]) { d[n] = s[n]; ++n; } d[n] = 0; return n; }
void lower(char* b, uint32_t n) { for (uint32_t i = 0; i < n; ++i) if (b[i] >= 'A' && b[i] <= 'Z') b[i] += 32; }
uint32_t tick() { return g_tick; }
void capture(uint32_t len, uint32_t, const uint8_t* p) { memcpy(g_out, p, len); g_outLen = len; }
void capture8(uint32_t len, uint8_t id, const uint8_t* p) { capture(len, id, p); }
int32_t capture_i(uint32_t len, uint8_t id, const uint8_t* p) { capture(len, id, p); return (int32_t)len; }
void no_wait(uint32_t) {}
}  // namespace

// ---- BNClient ----
struct Bn11960Env {
    uint32_t (*expansion)(); void (*lang)(uint8_t* p, uint32_t n); uint32_t (*local_addr)(uint32_t sock, uint8_t* addr, uint32_t* len);
    void (*system_time)(uint8_t* st); void (*local_time)(uint8_t* st); uint32_t (*to_filetime)(const uint8_t* st, uint32_t* ft);
    uint32_t (*user_lcid)(); uint32_t (*load_library)(const char* name); uint32_t (*get_proc)(uint32_t h, const char* name);
    uint32_t (*call_proc)(uint32_t p); void (*free_library)(uint32_t h); uint32_t (*lang_id)();
    void (*locale_info)(uint32_t lcid, uint32_t type, char* buf, uint32_t n); uint32_t (*slen)(const char* s);
    void (*send)(uint32_t len, uint32_t id, const uint8_t* p);
};
extern "C" uint32_t d2_bnclient_11960(const uint32_t* sock, const Bn11960Env* e);
struct BnC9e0Env { uint32_t (*strcopy)(char*, const char*, uint32_t); void (*lower)(char*, uint32_t); uint32_t (*tick)(); void (*send)(uint32_t, uint8_t, const uint8_t*); };
extern "C" uint32_t bnclient_c9e0(const BnC9e0Env* e, const char* name, const char* password, uint32_t srv, uint32_t* g3ee40, uint32_t* g3efc0);
struct BnC6e0Globals { uint32_t* handle; uint32_t* busy; uint32_t* pending; uint32_t* g3ee58; uint32_t* g3efd8; uint32_t* result; };
extern "C" uint32_t bnclient_send_bare_c6e0(const BnC6e0Globals* g, void (*copy)(uint8_t*, const uint8_t*, uint32_t),
                                            int32_t (*queue)(uint32_t, const uint8_t*, uint32_t), void (*wait)(uint32_t));
struct BnBfd0Env { uint32_t (*strcopy)(char*, const char*, uint32_t); int32_t (*send)(uint32_t, uint8_t, const uint8_t*); void (*wait)(uint32_t); };
struct BnBfd0Globals { uint32_t* g3f874; uint32_t* g3f87c; uint32_t* g3ee50; uint32_t* g3efd0; const uint32_t* g3f88c; };
extern "C" uint32_t bnclient_bfd0(const BnBfd0Env* e, const BnBfd0Globals* g, const char* title, const char* password);

// ---- D2MCPClient ----
extern "C" uint32_t d2_mcp_req_7510(const uint8_t* reply, uint8_t* out);
extern "C" uint32_t d2_mcp_req_6cb0(uint8_t* out);
extern "C" uint32_t d2_mcp_req_7410(const char* s, uint8_t* out);
extern "C" uint32_t d2_mcp_req_7330(const uint8_t* s, uint16_t w, uint8_t* out);
extern "C" uint32_t d2_mcp_req_7290(const char* s1, const char* s2, uint16_t w, uint8_t* out);

namespace {
uint32_t expansion_d2xp() { return 1; }
uint32_t expansion_d2dv() { return 0; }
void lang_none(uint8_t*, uint32_t) {}
uint32_t no_addr(uint32_t, uint8_t*, uint32_t*) { return 0; }
void st_fixed(uint8_t* st) { memset(st, 0, 16); }
uint32_t to_ft(const uint8_t*, uint32_t* ft) { ft[0] = ft[1] = 0; return 1; }
uint32_t lcid_us() { return 0x409; }
uint32_t load_k32(const char*) { return 1; }
uint32_t proc_ui(uint32_t, const char*) { return 1; }
uint32_t ui_lang(uint32_t) { return 0x409; }
void free_k32(uint32_t) {}
void locale(uint32_t, uint32_t type, char* b, uint32_t n) { sstrcopy(b, type == 7 ? "USA" : "United States", n); }
uint32_t slen(const char* s) { return (uint32_t)strlen(s); }
void copy_bytes(uint8_t* d, const uint8_t* s, uint32_t n) { memmove(d, s, n); }
int32_t queue_frame(uint32_t, const uint8_t* p, uint32_t n) { memcpy(g_out, p, n); g_outLen = n; return (int32_t)n; }

uint32_t mcpBody(uint32_t n, const uint8_t* frame, uint8_t* out) {   // drop the op byte the retail composer wrote first
    if (n == 0) return 0;
    memmove(out, frame + 1, n - 1);
    return n - 1;
}
}  // namespace

extern "C" {

// SID_AUTH_INFO (0x50) through BNClient+0x11960. `expansion` selects D2XP over D2DV.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_bncs_auth_info(int expansion, uint8_t* out) {
    const Bn11960Env e{expansion ? expansion_d2xp : expansion_d2dv, lang_none, no_addr, st_fixed, st_fixed, to_ft, lcid_us,
                       load_k32, proc_ui, ui_lang, free_k32, lcid_us, locale, slen, capture};
    const uint32_t sock = 0;
    g_out = out; g_outLen = 0;
    d2_bnclient_11960(&sock, &e);
    return (int)g_outLen;
}

// SID_LOGONRESPONSE2 (0x3A) through BNClient+0xC9E0; the tick the retail function reads is the client token.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_bncs_logon(uint32_t clientToken, uint32_t serverToken, const char* account, const char* password, uint8_t* out) {
    const BnC9e0Env e{sstrcopy, lower, tick, capture8};
    uint32_t g0 = 0, g1 = 0;
    g_out = out; g_outLen = 0; g_tick = clientToken;
    bnclient_c9e0(&e, account, password, serverToken, &g0, &g1);
    return (int)g_outLen;
}

// SID_QUERYREALMS2 (0x40) through BNClient+0xC6E0: the bare frame FF 40 04 00, so the body is empty.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_bncs_query_realms(uint8_t* out) {
    uint32_t handle = 1, busy = 0, pending = 0, a = 0, b = 0, result = 0;
    const BnC6e0Globals g{&handle, &busy, &pending, &a, &b, &result};
    uint8_t frame[8];
    g_out = frame; g_outLen = 0;
    bnclient_send_bare_c6e0(&g, copy_bytes, queue_frame, no_wait);
    if (g_outLen < 4 || frame[0] != 0xFF || frame[1] != 0x40) return -1;
    memcpy(out, frame + 4, g_outLen - 4);
    return (int)g_outLen - 4;
}

// SID_LOGONREALMEX (0x3E) through BNClient+0xBFD0; the token counter is seeded so the packet carries clientToken.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_bncs_logon_realm(uint32_t clientToken, uint32_t serverToken, const char* realm, const char* password, uint8_t* out) {
    uint32_t srv = serverToken, counter = clientToken - 1, a = 0, b = 0, result = 0;
    const BnBfd0Env e{sstrcopy, capture_i, no_wait};
    const BnBfd0Globals g{&srv, &counter, &a, &b, &result};
    g_out = out; g_outLen = 0;
    bnclient_bfd0(&e, &g, realm, password);
    return (int)g_outLen;
}

// MCP_STARTUP (0x01) through D2MCPClient+0x7510: reply is the LOGONREALMEX reply body (at least 0x48 bytes,
// then the unique name). Copied into a NUL-padded buffer so a reply without a terminator cannot over-read.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_mcp_startup(const uint8_t* reply, int len, uint8_t* out) {
    if (len < 0x48) return 0;
    static uint8_t buf[0x48 + 0x401], frame[0x48 + 0x401];
    const int n = len < (int)sizeof buf - 1 ? len : (int)sizeof buf - 1;
    memset(buf, 0, sizeof buf); memcpy(buf, reply, n);
    return (int)mcpBody(d2_mcp_req_7510(buf, frame), frame, out);
}

// MCP_CHARLIST2 (0x19) through D2MCPClient+0x6CB0: retail always asks for eight.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_mcp_charlist(uint8_t* out) {
    uint8_t frame[8];
    return (int)mcpBody(d2_mcp_req_6cb0(frame), frame, out);
}

// MCP_CHARLOGON (0x07) through D2MCPClient+0x7410.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_mcp_charlogon(const char* name, uint8_t* out) {
    static uint8_t frame[0x402];
    return (int)mcpBody(d2_mcp_req_7410(name, frame), frame, out);
}

// MCP_CREATEGAME (0x03) through D2MCPClient+0x7330. The create-game form holds three 16-byte slots (name,
// password, description), the u32 difficulty at +0x40, then the constant 1, the level difference (0xFF when the
// level limit box is off) and the player limit; name and password are cut at 15 bytes, the description at 31,
// like the dialog's edit fields.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_mcp_creategame(uint32_t requestId, uint32_t difficulty, uint32_t maxPlayers, uint32_t levelDifference,
                                                         const char* name, const char* password, const char* description, uint8_t* out) {
    alignas(4) uint8_t form[0x48] = {};
    sstrcopy((char*)form, name, 0x10);
    sstrcopy((char*)form + 0x10, password, 0x10);
    sstrcopy((char*)form + 0x20, description, 0x20);
    memcpy(form + 0x40, &difficulty, 4);
    form[0x44] = 1; form[0x45] = (uint8_t)levelDifference; form[0x46] = (uint8_t)maxPlayers;
    uint8_t frame[0x40];
    return (int)mcpBody(d2_mcp_req_7330(form, (uint16_t)requestId, frame), frame, out);
}

// MCP_JOINGAME (0x04) through D2MCPClient+0x7290.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_mcp_joingame(uint32_t requestId, const char* name, const char* password, uint8_t* out) {
    static uint8_t frame[0x404];
    return (int)mcpBody(d2_mcp_req_7290(name, password, (uint16_t)requestId, frame), frame, out);
}

}  // extern "C"
