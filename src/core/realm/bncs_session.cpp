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

extern "C" {
int d2_realm_bncs_header(uint8_t id, int bodyLen, uint8_t* out);
int d2_realm_bncs_auth_info_reply(uint32_t logonType, uint32_t serverToken, uint32_t udpValue, uint32_t filetimeLo, uint32_t filetimeHi, const char* mpqName, const char* challenge, uint8_t* out);
int d2_realm_bncs_auth_check_reply(uint32_t result, const char* info, uint8_t* out);
int d2_realm_bncs_logonresponse2_reply(uint32_t result, uint8_t* out);
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

inline uint32_t rd32(const uint8_t* p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

// Frame a composed body as [0xFF][id][len] ++ body, appending to `out`.
int emit(uint8_t id, const uint8_t* body, int bodyLen, uint8_t* out, int outCap, int written) {
    const int total = 4 + bodyLen;
    if (written + total > outCap) return -1;
    d2_realm_bncs_header(id, bodyLen, out + written);
    for (int i = 0; i < bodyLen; i++) out[written + 4 + i] = body[i];
    return written + total;
}

}  // namespace

extern "C" {

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

    uint8_t body[512];
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
            case 0x3A:   // LOGONRESPONSE2 -> accept
                n = d2_realm_bncs_logonresponse2_reply(0, body);
                written = emit(0x3A, body, n, out, outCap, written);
                break;
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
