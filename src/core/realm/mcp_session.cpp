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
int d2_realm_listed_game(const char* name, const char** pass, uint32_t* status);
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
    bool charDeleted;   // the one fixture character was removed by MCP_CHARDELETE
    bool charLoggedOn;  // it was logged on: the realm refuses to delete a character that is in use
    int error;
};
McpSession g_m;

const char kCharName[] = "player";

// MCP_CREATEGAME / MCP_JOINGAME results (D2Multi's two result switches; BNETDocs): 0 ok; create 0x1e bad name,
// 0x1f the name is taken; join 0x29 wrong password, 0x2a no such game, 0x2b the game is full.
constexpr uint32_t kCreateBadName = 0x1e, kCreateTaken = 0x1f, kJoinBadPassword = 0x29, kJoinNoGame = 0x2a, kJoinFull = 0x2b;
// The games players made on this realm. They outlive an MCP connection (a character leaves the lobby for the game and
// comes back), so a reset does not clear them.
struct MadeGame { char name[20]; char pass[20]; };
MadeGame g_made[16];
int g_madeCount = 0;
bool sameText(const char* a, const char* b) {
    for (; *a && *b; a++, b++) { char x = *a, y = *b; if (x >= 'A' && x <= 'Z') x += 32; if (y >= 'A' && y <= 'Z') y += 32; if (x != y) return false; }
    return !*a && !*b;
}
// Reads a NUL-terminated string out of [p, end); null when it runs off the body.
const char* readText(const uint8_t* p, const uint8_t* end, const uint8_t** next) {
    for (const uint8_t* q = p; q < end; q++) if (!*q) { *next = q + 1; return (const char*)p; }
    return nullptr;
}
const MadeGame* findMade(const char* name) {
    for (int i = 0; i < g_madeCount; i++) if (sameText(g_made[i].name, name)) return &g_made[i];
    return nullptr;
}
bool gameExists(const char* name) {
    const char* pass; uint32_t status;
    return findMade(name) || d2_realm_listed_game(name, &pass, &status) >= 0;
}
uint32_t createResult(const uint8_t* b, int bodyLen) {
    const uint8_t* end = b + bodyLen; const uint8_t* next;
    if (bodyLen < 10) return kCreateBadName;
    const char* name = readText(b + 9, end, &next);
    if (!name || !*name) return kCreateBadName;
    int len = 0; while (name[len]) len++;
    if (len > 15) return kCreateBadName;
    if (gameExists(name)) return kCreateTaken;
    const char* pass = readText(next, end, &next);
    if (g_madeCount < 16) {
        MadeGame& g = g_made[g_madeCount++];
        int i = 0; for (; i < 19 && name[i]; i++) g.name[i] = name[i]; g.name[i] = 0;
        int j = 0; for (; pass && j < 19 && pass[j]; j++) g.pass[j] = pass[j]; g.pass[j] = 0;
    }
    return 0;
}
uint32_t joinResult(const uint8_t* b, int bodyLen) {
    const uint8_t* end = b + bodyLen; const uint8_t* next = nullptr;
    const char* name = bodyLen > 2 ? readText(b + 2, end, &next) : nullptr;
    if (!name) return kJoinNoGame;
    const char* given = readText(next, end, &next); if (!given) given = "";
    const char* pass = ""; uint32_t status = 0;
    if (const MadeGame* g = findMade(name)) pass = g->pass;
    else if (d2_realm_listed_game(name, &pass, &status) < 0) return kJoinNoGame;
    if (*pass && !sameText(pass, given)) return kJoinBadPassword;
    return (status & 1) ? kJoinFull : 0;
}
constexpr uint32_t kCharLogonRefused = 0x46;
// MCP_CHARDELETE results: 0 deleted, 0x49 no such character (1.10+, tristramd mcp_chardelete.c), 1 the
// character is still in a game (d2-dedicated-server d2cs.zig refuses the same case).
constexpr uint32_t kCharDeleteGone = 0x49, kCharDeleteBusy = 1;

bool isListedCharacter(const uint8_t* name, int nameLen) {
    constexpr int expectedLen = sizeof(kCharName); // includes the required NUL
    if (g_m.charDeleted || !name || nameLen != expectedLen) return false;
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
    g_m.charDeleted = false;
    g_m.charLoggedOn = false;
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
                const uint16_t have = g_m.charDeleted ? 0 : 1;
                n = d2_realm_mcp_charlist2_reply_header(returnedLimit, have, have, body);
                if (have) n += d2_realm_mcp_charlist2_entry(0x7FFFFFFFu, kCharName, stat, sn, body + n);
                written = emit(0x19, body, n, out, outCap, written);
                break;
            }
            case 0x07:   // CHARLOGON -> select only the character this session advertised
                // The fixture currently has one character. Do not accept a different or
                // unterminated name as a successful selection. D2Client 1.13c recognizes 0x46
                // as a recoverable character-selection refusal, allowing a retry on this MCP
                // session. tristramd also names 0x7A for logon failure, but that is not a safe
                // refusal on the local 1.13c client path.
                if (isListedCharacter(b, bodyLen)) g_m.charLoggedOn = true;
                n = d2_realm_mcp_charlogon_reply(
                    isListedCharacter(b, bodyLen) ? 0u : kCharLogonRefused, body);
                written = emit(0x07, body, n, out, outCap, written);
                break;
            case 0x0A: { // CHARDELETE: the request id echoed, then a u32 result
                if (bodyLen < 3) { g_m.error = 1; return -1; }
                const uint16_t reqId = rd16(b);
                uint32_t result = kCharDeleteGone;
                if (isListedCharacter(b + 2, bodyLen - 2)) {
                    if (g_m.charLoggedOn) result = kCharDeleteBusy;
                    else { g_m.charDeleted = true; result = 0; }
                }
                body[0] = (uint8_t)reqId; body[1] = (uint8_t)(reqId >> 8);
                n = 2 + d2_realm_mcp_charlogon_reply(result, body + 2);   // same layout: a u32 LE
                written = emit(0x0A, body, n, out, outCap, written);
                break;
            }
            case 0x03: { // CREATEGAME -> created, hand back the token
                const uint16_t reqId = bodyLen >= 2 ? rd16(b) : 0;
                const uint32_t result = createResult(b, bodyLen);
                n = d2_realm_mcp_creategame_reply(reqId, result ? 0 : g_m.gameToken, result, body);
                written = emit(0x03, body, n, out, outCap, written);
                break;
            }
            case 0x04: { // JOINGAME -> the ticket: token, game server, hash
                const uint16_t reqId = bodyLen >= 2 ? rd16(b) : 0;
                const uint32_t result = joinResult(b, bodyLen);
                n = result ? d2_realm_mcp_joingame_reply(reqId, 0, 0, 0, result, body)
                           : d2_realm_mcp_joingame_reply(reqId, g_m.gameToken, g_m.gsIp, g_m.gameHash, 0, body);
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
