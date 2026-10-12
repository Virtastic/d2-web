// The MCP server session: the same pure bytes-in/bytes-out sequencing layer as bncs_session.cpp,
// for the realm leg. It answers STARTUP, CHARLIST2, CHARLOGON, CREATEGAME and JOINGAME, and it
// tolerates TCP chunking the same way -- buffer, consume only whole frames, keep the tail.
//
// MCP's frame differs from BNCS's in a way that is easy to get wrong: the length COUNTS ITSELF and
// the op id, so the body is `total - 3`, and there is no 0xFF marker to resynchronise on. A server
// that treats the length as a body length hangs the client waiting for three bytes that never come.
#include <cstdint>
#include <emscripten.h>

extern "C" {
int d2_realm_mcp_header(uint8_t op, int bodyLen, uint8_t* out);
int d2_realm_mcp_startup_reply(uint32_t result, uint8_t* out);
int d2_realm_mcp_charlist2_reply_header(uint16_t requested, uint32_t total, uint16_t returned, uint8_t* out);
int d2_realm_mcp_charlist2_entry(uint32_t expiry, const char* name, const uint8_t* statstring, int statLen, uint8_t* out);
int d2_realm_mcp_charlogon_reply(uint32_t result, uint8_t* out);
int d2_realm_mcp_creategame_reply(uint16_t requestId, uint16_t token, uint32_t result, uint8_t* out);
int d2_realm_mcp_joingame_reply(uint16_t requestId, uint16_t token, uint32_t gsIp, uint32_t hash, uint32_t result, uint8_t* out);
}

namespace {

constexpr int kRxCap = 8192;

struct McpSession {
    uint8_t rx[kRxCap];
    int rxLen;
    bool selectorSeen;
    uint32_t gsIp;
    uint16_t gameToken;
    uint32_t gameHash;
    uint8_t charClass;
    uint8_t charLevel;
    int error;
};
McpSession g_m;

const char kCharName[] = "player";
constexpr uint32_t kCharLogonRefused = 0x46;

bool isListedCharacter(const uint8_t* name, int nameLen) {
    constexpr int expectedLen = sizeof(kCharName); // includes the required NUL
    if (!name || nameLen != expectedLen) return false;
    for (int i = 0; i < expectedLen; ++i) {
        if (name[i] != static_cast<uint8_t>(kCharName[i])) return false;
    }
    return true;
}

inline uint16_t rd16(const uint8_t* p) { return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8)); }
inline uint32_t rd32(const uint8_t* p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

int emit(uint8_t op, const uint8_t* body, int bodyLen, uint8_t* out, int outCap, int written) {
    const int total = 3 + bodyLen;
    if (written + total > outCap) return -1;
    d2_realm_mcp_header(op, bodyLen, out + written);
    for (int i = 0; i < bodyLen; i++) out[written + 3 + i] = body[i];
    return written + total;
}

// The char-select statstring is where class and level actually live: the client reads the class at
// byte 13 (stored +1) and the level at byte 25, so anything shorter than 26 bytes silently yields a
// level-0 character rather than an error.
//
// It is also carried as a NULL-TERMINATED string, which constrains its contents: an embedded zero
// ends it early, and a zero in byte 0 makes it the empty string. Filling the unused bytes with 0
// (the obvious choice) therefore puts an EMPTY statstring on the wire -- the fields are all still
// "there", but nothing downstream ever reads them, and the character arrives classless and level 0
// with no error anywhere. The two references disagree on this: diablo2-protocol's 1.13 schema types
// it `string` (cstring), while d2-clientless indexes the raw bytes; filling with a non-zero byte
// satisfies both readings at once, so that is what we do.
int buildStatstring(uint8_t cls, uint8_t level, uint8_t* out) {
    for (int i = 0; i < 32; i++) out[i] = 0x20;   // non-zero filler: see above
    out[13] = (uint8_t)(cls + 1);
    out[25] = level;
    return 32;
}

}  // namespace

extern "C" {

EMSCRIPTEN_KEEPALIVE void d2_realm_mcp_session_reset(uint32_t gsIp, uint16_t gameToken, uint32_t gameHash, uint8_t charClass, uint8_t charLevel) {
    g_m.rxLen = 0;
    g_m.selectorSeen = false;
    g_m.gsIp = gsIp;
    g_m.gameToken = gameToken;
    g_m.gameHash = gameHash;
    g_m.charClass = charClass;
    g_m.charLevel = charLevel;
    g_m.error = 0;
}

EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_session_pending() { return g_m.rxLen; }
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_session_error() { return g_m.error; }

// Feed inbound bytes; returns reply bytes written to `out`, or -1 on a protocol error / overflow.
EMSCRIPTEN_KEEPALIVE int d2_realm_mcp_session_feed(const uint8_t* in, int len, uint8_t* out, int outCap) {
    if (g_m.error) return -1;
    if (g_m.rxLen + len > kRxCap) { g_m.error = 1; return -1; }
    for (int i = 0; i < len; i++) g_m.rx[g_m.rxLen++] = in[i];

    int written = 0;
    int off = 0;
    if (!g_m.selectorSeen) {
        if (g_m.rxLen - off < 1) return 0;
        if (g_m.rx[off] != 0x01) { g_m.error = 1; return -1; }
        off += 1;
        g_m.selectorSeen = true;
    }

    uint8_t body[512];
    uint8_t stat[32];
    while (true) {
        if (g_m.rxLen - off < 3) break;
        const uint8_t* f = g_m.rx + off;
        const int total = (int)f[0] | ((int)f[1] << 8);   // counts itself and the op
        if (total < 3) { g_m.error = 1; return -1; }
        if (g_m.rxLen - off < total) break;               // wait for the rest of this frame
        const uint8_t op = f[2];
        const uint8_t* b = f + 3;
        const int bodyLen = total - 3;

        int n = 0;
        switch (op) {
            case 0x01:   // STARTUP -> session accepted
                n = d2_realm_mcp_startup_reply(0, body);
                written = emit(0x01, body, n, out, outCap, written);
                break;
            case 0x19: { // CHARLIST2 -> one character
                const int sn = buildStatstring(g_m.charClass, g_m.charLevel, stat);
                // The client sends a little-endian DWORD, with a protocol maximum of eight.
                // Reading only b[0] treated valid counts such as 0x00000100 as zero.
                const uint32_t requested = bodyLen >= 4 ? rd32(b) : 8;
                const uint16_t returnedLimit = (uint16_t)(requested > 8 ? 8 : requested);
                n = d2_realm_mcp_charlist2_reply_header(returnedLimit, 1, 1, body);
                n += d2_realm_mcp_charlist2_entry(0x7FFFFFFFu, kCharName, stat, sn, body + n);
                written = emit(0x19, body, n, out, outCap, written);
                break;
            }
            case 0x07:   // CHARLOGON -> select only the character this session advertised
                // The fixture currently has one character. Do not accept a different or
                // unterminated name as a successful selection. D2Client 1.13c recognizes 0x46
                // as a recoverable character-selection refusal, allowing a retry on this MCP
                // session. tristramd also names 0x7A for logon failure, but that is not a safe
                // refusal on the local 1.13c client path.
                n = d2_realm_mcp_charlogon_reply(
                    isListedCharacter(b, bodyLen) ? 0u : kCharLogonRefused, body);
                written = emit(0x07, body, n, out, outCap, written);
                break;
            case 0x03: { // CREATEGAME -> created, hand back the token
                const uint16_t reqId = bodyLen >= 2 ? rd16(b) : 0;
                n = d2_realm_mcp_creategame_reply(reqId, g_m.gameToken, 0, body);
                written = emit(0x03, body, n, out, outCap, written);
                break;
            }
            case 0x04: { // JOINGAME -> the ticket: token, game server, hash
                const uint16_t reqId = bodyLen >= 2 ? rd16(b) : 0;
                n = d2_realm_mcp_joingame_reply(reqId, g_m.gameToken, g_m.gsIp, g_m.gameHash, 0, body);
                written = emit(0x04, body, n, out, outCap, written);
                break;
            }
            default:
                break;   // MOTD and friends are not part of the logon order; ignore
        }
        if (written < 0) { g_m.error = 1; return -1; }
        off += total;
    }

    const int rest = g_m.rxLen - off;
    for (int i = 0; i < rest; i++) g_m.rx[i] = g_m.rx[off + i];
    g_m.rxLen = rest;
    return written;
}

}  // extern "C"
