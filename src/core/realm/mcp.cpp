// MCP (realm) protocol framing -- the leg between BNCS login and a D2GS game: listing and creating
// characters, creating or joining a game, and being handed the game server to dial. The frame is a
// little-endian length that COUNTS ITSELF and the op id, then the op id, then the body -- a server
// that writes the body length instead leaves the client waiting for three bytes that never come.
// The opcodes and the fixed logon order (STARTUP -> CHARLIST2 -> CHARLOGON, no MOTD between) are the
// vocabulary both ends must agree on; layouts are the community defs (libd2 bnet.mcp, d2-clientless).
// This slice is the framing + op vocabulary; the message bodies and BNCS crypto are later slices.
// 
#include <cstdint>
#include <emscripten.h>

// MCP message ids (one space, used both directions: a reply carries the request's id).
enum : uint8_t {
    MCP_STARTUP = 0x01,
    MCP_CHARCREATE = 0x02,
    MCP_CREATEGAME = 0x03,
    MCP_JOINGAME = 0x04,
    MCP_GAMELIST = 0x05,
    MCP_GAMEINFO = 0x06,
    MCP_CHARLOGON = 0x07,
    MCP_CHARDELETE = 0x0A,
    MCP_MOTD = 0x12,
    MCP_CHARLIST2 = 0x19,
};

extern "C" {

// MCP frame header: [total:u16 LE][op:u8], where total counts the 3-byte header and the body.
// Writes the 3 header bytes to `out`; returns the total frame length (3 + bodyLen), or -1 on
// overflow / a negative body.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_header(uint8_t op, int bodyLen, uint8_t* out) {
    const int total = 3 + bodyLen;
    if (bodyLen < 0 || total > 0xFFFF) return -1;
    out[0] = (uint8_t)total;
    out[1] = (uint8_t)(total >> 8);
    out[2] = op;
    return total;
}

// Decode the first three bytes of an MCP frame: returns (total << 8) | op, or -1 if fewer than 3
// bytes are present.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_decode(const uint8_t* buf, int len) {
    if (len < 3) return -1;
    const int total = buf[0] | (buf[1] << 8);
    return (total << 8) | buf[2];
}

// The three ids of the fixed logon order, packed big-to-small: (startup << 16) | (charlist2 << 8) |
// charlogon.  The order is a property of the session and not free-form: MOTD is never in it.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_logon_order() {
    return (MCP_STARTUP << 16) | (MCP_CHARLIST2 << 8) | MCP_CHARLOGON;
}

// ── The logon leg: STARTUP -> CHARLIST2 -> CHARLOGON (the fixed order above) ────────────────
// The MCP socket opens with a single 0x01 protocol-selector byte before any frame.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_protocol_selector(uint8_t* out) { out[0] = 0x01; return 1; }

// MCP_STARTUP (0x01) request body: 64 bytes forwarded verbatim out of the BNCS realm-logon reply --
// cookie+status (its first 16 bytes) then the 48-byte chunk at its offset 24 (d2-clientless
// realm.zig).  Short input is zero-filled.  Returns 64.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_startup_request(const uint8_t* realmReply, int len, uint8_t* out) {
    for (int i = 0; i < 64; i++) out[i] = 0;
    for (int i = 0; i < 16 && i < len; i++) out[i] = realmReply[i];
    for (int i = 0; i < 48 && 24 + i < len; i++) out[16 + i] = realmReply[24 + i];
    return 64;
}

// MCP_STARTUP (0x01) reply body: result(u32 LE) -- 0 accepted, 0x02 rejected (tristramd
// mcp_startup.c).  4 bytes.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_startup_reply(uint32_t result, uint8_t* out) {
    out[0] = (uint8_t)result; out[1] = (uint8_t)(result >> 8); out[2] = (uint8_t)(result >> 16); out[3] = (uint8_t)(result >> 24);
    return 4;
}

// MCP_CHARLIST2 (0x19) request body: how many characters to list (u32 LE; the client asks for 8).
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_charlist2_request(uint32_t count, uint8_t* out) {
    out[0] = (uint8_t)count; out[1] = (uint8_t)(count >> 8); out[2] = (uint8_t)(count >> 16); out[3] = (uint8_t)(count >> 24);
    return 4;
}

// MCP_CHARLIST2 (0x19) reply header: requested(u16), total(u32), returned(u16) -- the character
// entries follow (tristramd mcp_charlist2.c).  8 bytes.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_charlist2_reply_header(uint16_t requested, uint32_t total, uint16_t returned, uint8_t* out) {
    out[0] = (uint8_t)requested; out[1] = (uint8_t)(requested >> 8);
    out[2] = (uint8_t)total; out[3] = (uint8_t)(total >> 8); out[4] = (uint8_t)(total >> 16); out[5] = (uint8_t)(total >> 24);
    out[6] = (uint8_t)returned; out[7] = (uint8_t)(returned >> 8);
    return 8;
}

// One MCP_CHARLIST2 entry: expiry(u32 LE), name (null-terminated), then the char-select statstring
// and its terminator.  The statstring is where class and level actually live -- the realm never
// states them outright.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_charlist2_entry(uint32_t expiry, const char* name, const uint8_t* statstring, int statLen, uint8_t* out) {
    int w = 0;
    out[w++] = (uint8_t)expiry; out[w++] = (uint8_t)(expiry >> 8); out[w++] = (uint8_t)(expiry >> 16); out[w++] = (uint8_t)(expiry >> 24);
    for (const char* p = name; p && *p; p++) out[w++] = (uint8_t)*p;
    out[w++] = 0;
    for (int i = 0; i < statLen; i++) out[w++] = statstring[i];
    out[w++] = 0;
    return w;
}

// MCP_CHARLOGON (0x07) request body: the character name, null-terminated.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_charlogon_request(const char* name, uint8_t* out) {
    int w = 0;
    for (const char* p = name; p && *p; p++) out[w++] = (uint8_t)*p;
    out[w++] = 0;
    return w;
}

// MCP_CHARLOGON (0x07) reply body: result(u32 LE) -- 0 logged on, 0x7A failed (tristramd
// mcp_charlogon.c).  4 bytes.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_charlogon_reply(uint32_t result, uint8_t* out) {
    out[0] = (uint8_t)result; out[1] = (uint8_t)(result >> 8); out[2] = (uint8_t)(result >> 16); out[3] = (uint8_t)(result >> 24);
    return 4;
}

// MCP_CREATEGAME (0x03) request body: request_id(u16=1), difficulty(u32), 1 (always), 0 (player
// difficulty), maxPlayers(u8), then name / password / description as null-terminated strings
// (d2-clientless realm.zig createGame).  Writes the body to `out`; returns its length.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_creategame_request(uint32_t difficulty, uint8_t maxPlayers, const char* name, const char* password, const char* description, uint8_t* out) {
    int w = 0;
    out[w++] = 0x01; out[w++] = 0x00;   // request id = 1
    out[w++] = (uint8_t)difficulty; out[w++] = (uint8_t)(difficulty >> 8); out[w++] = (uint8_t)(difficulty >> 16); out[w++] = (uint8_t)(difficulty >> 24);
    out[w++] = 1;   // always 1
    out[w++] = 0;   // player difficulty
    out[w++] = maxPlayers;
    for (const char* p = name; p && *p; p++) out[w++] = (uint8_t)*p;
    out[w++] = 0;
    for (const char* p = password; p && *p; p++) out[w++] = (uint8_t)*p;
    out[w++] = 0;
    for (const char* p = description; p && *p; p++) out[w++] = (uint8_t)*p;
    out[w++] = 0;
    return w;
}

// MCP_JOINGAME (0x04) request body: request_id(u16=2), then name / password as null-terminated
// strings (d2-clientless realm.zig joinGame).  Writes the body to `out`; returns its length.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_joingame_request(const char* name, const char* password, uint8_t* out) {
    int w = 0;
    out[w++] = 0x02; out[w++] = 0x00;   // request id = 2
    for (const char* p = name; p && *p; p++) out[w++] = (uint8_t)*p;
    out[w++] = 0;
    for (const char* p = password; p && *p; p++) out[w++] = (uint8_t)*p;
    out[w++] = 0;
    return w;
}

// MCP_CREATEGAME (0x03) reply body: request_id(u16), token(u16), unknown(u16=0), result(u32) --
// all little-endian (tristramd mcp_creategame.c).  result 0 is created; token is 0 on failure.
// 10 bytes.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_creategame_reply(uint16_t requestId, uint16_t token, uint32_t result, uint8_t* out) {
    out[0] = (uint8_t)requestId; out[1] = (uint8_t)(requestId >> 8);
    out[2] = (uint8_t)token; out[3] = (uint8_t)(token >> 8);
    out[4] = 0; out[5] = 0;
    out[6] = (uint8_t)result; out[7] = (uint8_t)(result >> 8); out[8] = (uint8_t)(result >> 16); out[9] = (uint8_t)(result >> 24);
    return 10;
}

// MCP_JOINGAME (0x04) reply body: request_id(u16 LE), token(u16 LE), unknown(u16 LE=0), game-server
// IP(u32 BIG-endian / network order), hash(u32 LE), result(u32 LE) (tristramd mcp_joingame.c).
// result 0 is ok; token is 0 on failure.  18 bytes.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_joingame_reply(uint16_t requestId, uint16_t token, uint32_t gsIp, uint32_t hash, uint32_t result, uint8_t* out) {
    out[0] = (uint8_t)requestId; out[1] = (uint8_t)(requestId >> 8);
    out[2] = (uint8_t)token; out[3] = (uint8_t)(token >> 8);
    out[4] = 0; out[5] = 0;
    out[6] = (uint8_t)(gsIp >> 24); out[7] = (uint8_t)(gsIp >> 16); out[8] = (uint8_t)(gsIp >> 8); out[9] = (uint8_t)gsIp;   // network order
    out[10] = (uint8_t)hash; out[11] = (uint8_t)(hash >> 8); out[12] = (uint8_t)(hash >> 16); out[13] = (uint8_t)(hash >> 24);
    out[14] = (uint8_t)result; out[15] = (uint8_t)(result >> 8); out[16] = (uint8_t)(result >> 16); out[17] = (uint8_t)(result >> 24);
    return 18;
}

}  // extern "C"
