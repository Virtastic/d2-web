// The BNCS server session: the sequencing layer between the message composers (bncs.cpp) and a
// socket. It is deliberately pure -- bytes in, bytes out, no I/O -- so the socket layer above it is
// a thin read -> feed -> write shim and all the ordering logic stays testable in the test runner.
//
// The property that matters for a real socket is that TCP hands over arbitrary chunks: a frame can
// arrive split across reads, or several frames can arrive in one. So `feed` buffers, consumes only
// whole frames, and leaves any tail for the next call. s123 drives the same session both in one
// shot and one byte at a time and requires identical output.
#include <cstdint>
#include <emscripten.h>
#include <initializer_list>

extern "C" {
int d2_realm_bncs_header(uint8_t id, int bodyLen, uint8_t* out);
int d2_realm_bncs_auth_info_reply(uint32_t logonType, uint32_t serverToken, uint32_t udpValue, uint32_t filetimeLo, uint32_t filetimeHi, const char* mpqName, const char* challenge, uint8_t* out);
int d2_realm_bncs_auth_check_reply(uint32_t result, const char* info, uint8_t* out);
int d2_realm_bncs_logonresponse2_reply(uint32_t result, uint8_t* out);
int d2_realm_bncs_changepassword_reply(uint32_t result, uint8_t* out);
void d2_realm_bncs_double_hash(uint32_t clientToken, uint32_t serverToken, const uint8_t* pwhash20, uint8_t* out20);
int d2_realm_bncs_queryrealms2_reply(const char* title, const char* description, uint8_t* out);
int d2_realm_bncs_logonrealmex_reply(uint32_t clientToken, uint32_t status, uint32_t ip, uint32_t uuid, uint16_t port, const char* uniqueName, uint8_t* out);
int d2_realm_bncs_ping(uint32_t cookie, uint8_t* out);
}

namespace {

constexpr int kRxCap = 8192;

struct BncsSession {
    uint8_t rx[kRxCap];
    int rxLen;
    bool selectorSeen;
    uint32_t serverToken;
    uint32_t uuid;
    uint32_t ip;
    uint16_t port;
    int error;
};
BncsSession g_s;

// Defaults for a private realm; the CheckRevision challenge is the classic formula, which a
// permissive realm accepts any answer to.
const char kMpq[] = "ver-IX86-5.mpq";
const char kChallenge[] = "A=1 B=1 C=1 4 A=A-S B=B-C C=C-A A=A^B";
const char kRealmTitle[] = "d2-web";
const char kRealmDesc[] = "private realm";
const char kUniqueName[] = "player";
const char kChatPeer[] = "Deckard";

inline uint32_t rd32(const uint8_t* p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

// Frame a composed body as [0xFF][id][len] ++ body, appending to `out`.
int emit(uint8_t id, const uint8_t* body, int bodyLen, uint8_t* out, int outCap, int written) {
    const int total = 4 + bodyLen;
    if (written + total > outCap) return -1;
    d2_realm_bncs_header(id, bodyLen, out + written);
    for (int i = 0; i < bodyLen; i++) out[written + 4 + i] = body[i];
    return written + total;
}

// The accounts this process has seen created. Accounts nobody created are accepted as before (a permissive
// private realm); a created one is held to its password, and SID_CHANGEPASSWORD replaces it. Process lifetime,
// not per session, so a change survives the reconnect the next log on makes.
struct Account { char name[40]; uint8_t hash[20]; char email[64]; bool used; };
constexpr int kAccounts = 64;
Account g_accounts[kAccounts];

char lower(char c) { return (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c; }
bool sameName(const char* a, const char* b) {
    for (;; a++, b++) { if (lower(*a) != lower(*b)) return false; if (!*a) return true; }
}
Account* findAccount(const char* name) {
    for (auto& a : g_accounts) if (a.used && sameName(a.name, name)) return &a;
    return nullptr;
}
Account* addAccount(const char* name, const uint8_t* hash) {
    Account* a = findAccount(name);
    if (!a) { for (auto& s : g_accounts) if (!s.used) { a = &s; break; } }
    if (!a) return nullptr;
    int i = 0;
    for (; name[i] && i < (int)sizeof a->name - 1; i++) a->name[i] = name[i];
    a->name[i] = 0;
    for (int j = 0; j < 20; j++) a->hash[j] = hash[j];
    a->used = true;
    return a;
}
// A NUL-terminated string inside [p, end); null when it runs off the body.
const char* cstr(const uint8_t* p, const uint8_t* end, const uint8_t** next) {
    for (const uint8_t* q = p; q < end; q++) if (!*q) { *next = q + 1; return (const char*)p; }
    return nullptr;
}

// The games the test double lists for SID_GETADVLISTEX (0x09): a fixed table, one hardcore, one with a password,
// so a client's own filter has something to hide. Reply: dword count; per game a 0x20 header (type, ext, language,
// family, port, address at +0xC, status at +0x18, uptime at +0x1C), name\0 password\0 statstring\0 (the statstring's
// first char the difficulty, the rest the description).
struct ListedGame { const char* name; const char* pass; char diff; const char* desc; uint32_t status; };
const ListedGame kGames[] = {
    { "Baal Runs 24-7", "", '2', "fast baal runs only", 0 },
    { "cows", "", '0', "cow level", 0 },
    { "NM Mephisto", "secret", '1', "mephisto nightmare, password", 4 },
    { "Hell Ubers", "", '2', "ubers wanted, bring keys", 0 },
    { "HC Normal", "", '3', "hardcore, hidden from softcore", 0 },
    { "Tristram", "", '0', "", 0 },
    { "Full House", "", '1', "8 of 8 players", 1 },
    { "a", "x", '0', "short name, a password", 0 },
};
void put16(uint8_t* p, uint32_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
void put32(uint8_t* p, uint32_t v) { put16(p, v); put16(p + 2, v >> 16); }
int putStr(uint8_t* p, const char* s) { int n = 0; for (; s[n]; n++) p[n] = (uint8_t)s[n]; p[n] = 0; return n + 1; }
int composeGameList(uint8_t* out, int cap) {
    int n = 4, count = 0;
    for (const ListedGame& g : kGames) {
        int need = 0x20 + 3;
        for (const char* s : { g.name, g.pass, g.desc }) { int l = 0; while (s[l]) l++; need += l; }
        if (n + need > cap) break;
        uint8_t* e = out + n;
        for (int i = 0; i < 0x20; i++) e[i] = 0;
        put16(e, 1); put32(e + 4, 0x409); put16(e + 8, 2); put16(e + 10, 0xE017);
        put32(e + 0xC, 0x010200C0u + (uint32_t)count * 0x01000000u); put32(e + 0x18, g.status); put32(e + 0x1C, 100u * (uint32_t)(count + 1));
        uint8_t* q = e + 0x20; q += putStr(q, g.name); q += putStr(q, g.pass);
        *q++ = (uint8_t)g.diff; q += putStr(q, g.desc);
        n = (int)(q - out); count++;
    }
    put32(out, (uint32_t)count);
    return n;
}
// SID_CHATEVENT (0x0F) body: event, flags, ping, ip, account number, registration authority (dwords), then
// the user name and the text, each NUL-terminated. Returns its length.
int composeChatEvent(uint8_t* out, uint32_t event, uint32_t flags, const char* user, const char* text) {
    for (int i = 0; i < 24; i++) out[i] = 0;
    put32(out, event); put32(out + 4, flags);
    int n = 24;
    n += putStr(out + n, user);
    n += putStr(out + n, text);
    return n;
}
// The tokens plus the 20 byte proof at the head of 0x3A and 0x31.
bool proofMatches(const Account& a, const uint8_t* b) {
    uint8_t want[20];
    d2_realm_bncs_double_hash(rd32(b), rd32(b + 4), a.hash, want);
    for (int i = 0; i < 20; i++) if (want[i] != b[8 + i]) return false;
    return true;
}

}  // namespace

extern "C" {
// The MCP double's view of the games above: the index of the game called `name` (ignoring case), or -1; its
// password and its status (bit 0 set: full) come back through the pointers.
int d2_realm_listed_game(const char* name, const char** pass, uint32_t* status) {
    for (int i = 0; i < (int)(sizeof kGames / sizeof kGames[0]); i++) {
        const char* a = kGames[i].name; const char* b = name;
        for (; *a && *b; a++, b++) { char x = *a, y = *b; if (x >= 'A' && x <= 'Z') x += 32; if (y >= 'A' && y <= 'Z') y += 32; if (x != y) break; }
        if (*a || *b) continue;
        *pass = kGames[i].pass; *status = kGames[i].status; return i;
    }
    return -1;
}

// Start (or restart) the session. `serverToken` is what AUTH_INFO hands the client and what its
// login hash is computed over, so it is fixed per connection.
EMSCRIPTEN_KEEPALIVE void d2_realm_bncs_session_reset(uint32_t serverToken, uint32_t uuid, uint32_t ip, uint16_t port) {
    g_s.rxLen = 0;
    g_s.selectorSeen = false;
    g_s.serverToken = serverToken;
    g_s.uuid = uuid;
    g_s.ip = ip;
    g_s.port = port;
    g_s.error = 0;
}

// Bytes the session has buffered but not yet consumed (an incomplete frame waiting for more TCP).
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_session_pending() { return g_s.rxLen; }
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_session_error() { return g_s.error; }

// Feed inbound bytes; returns how many reply bytes were written to `out`, or -1 on a protocol
// error (bad selector, non-0xFF frame) or overflow.  Partial frames are buffered, not rejected.
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_session_feed(const uint8_t* in, int len, uint8_t* out, int outCap) {
    if (g_s.error) return -1;
    if (g_s.rxLen + len > kRxCap) { g_s.error = 1; return -1; }
    for (int i = 0; i < len; i++) g_s.rx[g_s.rxLen++] = in[i];

    int written = 0;
    int off = 0;
    if (!g_s.selectorSeen) {
        if (g_s.rxLen - off < 1) return 0;
        if (g_s.rx[off] != 0x01) { g_s.error = 1; return -1; }
        off += 1;
        g_s.selectorSeen = true;
    }

    uint8_t body[1024];
    while (true) {
        if (g_s.rxLen - off < 4) break;
        const uint8_t* f = g_s.rx + off;
        if (f[0] != 0xFF) { g_s.error = 1; return -1; }
        const int total = (int)f[2] | ((int)f[3] << 8);
        if (total < 4) { g_s.error = 1; return -1; }
        if (g_s.rxLen - off < total) break;   // wait for the rest of this frame
        const uint8_t id = f[1];
        const uint8_t* b = f + 4;
        const int bodyLen = total - 4;

        int n = 0;
        switch (id) {
            case 0x50:   // AUTH_INFO -> name the challenge
                n = d2_realm_bncs_auth_info_reply(0, g_s.serverToken, 1, 0x82b8d101u, 0x0004e292u, kMpq, kChallenge, body);
                written = emit(0x50, body, n, out, outCap, written);
                break;
            case 0x51:   // AUTH_CHECK -> permissive: passed
                n = d2_realm_bncs_auth_check_reply(0, "", body);
                written = emit(0x51, body, n, out, outCap, written);
                break;
            case 0x3A: { // LOGONRESPONSE2: accept, except a created account given the wrong proof (2)
                uint32_t result = 0;
                const uint8_t* nx;
                const char* name = bodyLen >= 29 ? cstr(b + 28, b + bodyLen, &nx) : nullptr;
                const Account* a = name ? findAccount(name) : nullptr;
                if (a && !proofMatches(*a, b)) result = 2;
                n = d2_realm_bncs_logonresponse2_reply(result, body);
                written = emit(0x3A, body, n, out, outCap, written);
                break;
            }
            case 0x3D: { // CREATEACCOUNT2: hash[20] ++ name; 0 created, 1 no usable name, 4 the name exists, 7 refused
                const uint8_t* nx;
                const char* name = bodyLen >= 21 ? cstr(b + 20, b + bodyLen, &nx) : nullptr;
                uint32_t result = 7;
                if (!name || !*name) result = 1;
                else result = findAccount(name) ? 4 : (addAccount(name, b) ? 0 : 7);
                n = d2_realm_bncs_changepassword_reply(result, body);
                written = emit(0x3D, body, n, out, outCap, written);
                break;
            }
            case 0x31: { // CHANGEPASSWORD: ct, st, old proof[20], new hash[20], name. Reply 1 changed, 0 wrong old password
                const uint8_t* nx;
                const char* name = bodyLen >= 49 ? cstr(b + 48, b + bodyLen, &nx) : nullptr;
                uint32_t result = 0;
                if (name && *name) {
                    const Account* a = findAccount(name);
                    // An account nobody created here has no password to check against: accepted, as 0x3A does.
                    if (!a || proofMatches(*a, b)) result = addAccount(name, b + 28) ? 1 : 0;
                }
                n = d2_realm_bncs_changepassword_reply(result, body);
                written = emit(0x31, body, n, out, outCap, written);
                break;
            }
            case 0x5A:   // RESETPASSWORD: account, email. Battle.net answers with mail, never a packet.
                break;
            case 0x5B: { // CHANGEEMAIL: account, old email, new email. No reply; a created account keeps the new address.
                const uint8_t* p = b;
                const uint8_t* e = b + bodyLen;
                const char* name = cstr(p, e, &p);
                const char* oldEmail = name ? cstr(p, e, &p) : nullptr;
                const char* newEmail = oldEmail ? cstr(p, e, &p) : nullptr;
                Account* a = newEmail ? findAccount(name) : nullptr;
                if (a) { int i = 0; for (; newEmail[i] && i < (int)sizeof a->email - 1; i++) a->email[i] = newEmail[i]; a->email[i] = 0; }
                break;
            }
            case 0x40:   // QUERYREALMS2 -> our single realm
                n = d2_realm_bncs_queryrealms2_reply(kRealmTitle, kRealmDesc, body);
                written = emit(0x40, body, n, out, outCap, written);
                break;
            case 0x3E: { // LOGONREALMEX -> the MCP handoff, echoing the client's cookie
                const uint32_t clientToken = bodyLen >= 4 ? rd32(b) : 0;
                n = d2_realm_bncs_logonrealmex_reply(clientToken, 1, g_s.ip, g_s.uuid, g_s.port, kUniqueName, body);
                written = emit(0x3E, body, n, out, outCap, written);
                break;
            }
            case 0x09:   // GETADVLISTEX: the fixed game table (the client filters it)
                n = composeGameList(body, (int)sizeof body);
                written = emit(0x09, body, n, out, outCap, written);
                break;
            case 0x0A: { // ENTERCHAT: character, realm. Answer the unique name, an empty statstring and the account name.
                const uint8_t* nx;
                const char* name = cstr(b, b + bodyLen, &nx);
                if (!name || !*name) name = kUniqueName;
                int w = putStr(body, name);
                w += putStr(body + w, "");
                w += putStr(body + w, name);
                written = emit(0x0A, body, w, out, outCap, written);
                break;
            }
            case 0x0C: { // JOINCHANNEL: flags dword, channel. Retail's reply is the joined-channel event, then the channel's users.
                const uint8_t* nx;
                const char* chan = bodyLen > 4 ? cstr(b + 4, b + bodyLen, &nx) : nullptr;
                if (!chan || !*chan || rd32(b) == 5) chan = "Diablo II";
                n = composeChatEvent(body, 0x07, 0, "", chan);
                written = emit(0x0F, body, n, out, outCap, written);
                if (written >= 0) {
                    n = composeChatEvent(body, 0x01, 0, kChatPeer, "");
                    written = emit(0x0F, body, n, out, outCap, written);
                }
                break;
            }
            case 0x0E: { // CHATCOMMAND: this realm has one other user, who answers every line with the same text
                const uint8_t* nx;
                const char* text = cstr(b, b + bodyLen, &nx);
                if (text && *text && text[0] != '/') {
                    n = composeChatEvent(body, 0x05, 0, kChatPeer, text);
                    written = emit(0x0F, body, n, out, outCap, written);
                }
                break;
            }
            case 0x25:   // PING -> echo the cookie back verbatim
                written = emit(0x25, b, bodyLen, out, outCap, written);
                break;
            default:
                break;   // unknown opcodes are ignored, as a permissive realm does
        }
        if (written < 0) { g_s.error = 1; return -1; }
        off += total;
    }

    // Keep the unconsumed tail for the next read.
    const int rest = g_s.rxLen - off;
    for (int i = 0; i < rest; i++) g_s.rx[i] = g_s.rx[off + i];
    g_s.rxLen = rest;
    return written;
}

}  // extern "C"
