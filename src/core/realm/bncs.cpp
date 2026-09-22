// BNCS (Battle.net Chat Server) protocol framing -- the login leg, ahead of MCP and D2GS. Every
// BNCS message is [0xFF][id:u8][len:u16 LE], where the length COUNTS the 4-byte header and the body
// (d2-clientless realm.zig: `[0xFF][id][u16 len incl header][body]`). The server pings with SID_PING
// (0x25) carrying a u32 cookie the client echoes back verbatim. This slice is the framing + the
// ping echo; the auth handshake (SID_AUTH_INFO/CHECK, SID_LOGONRESPONSE2, SID_LOGONREALMEX) and its
// SRP/CD-key/CheckRevision crypto (libd2 bnet.xsha1/cdkey/checkrev) are later slices. See
#include <cstdint>
#include <cctype>
#include <cstring>
#include <emscripten.h>

namespace {
// Battle.net's "Broken SHA-1" (xSHA-1) -- the OLS account-password hash, RE'd from Game.exe 1.14d
// D2Client SHA1 @0x5209d0 (libd2 bnet.xsha1, verified bit-for-bit against a real login). NOT FIPS
// SHA-1: the 16 message words are read little-endian, the expansion sets a single bit
// (rotl(1, (xor)&0x1f) instead of rotl(xor,1)), there is no 0x80/length padding (zero-fill one
// 64-byte block, inputs are always <= 64 bytes), and h0..h4 are written little-endian.
inline uint32_t rotl32(uint32_t x, unsigned n) { return n ? ((x << n) | (x >> (32 - n))) : x; }
void xsha1(const uint8_t* data, int len, uint8_t* out) {
    uint8_t block[64] = {0};
    for (int i = 0; i < len && i < 64; i++) block[i] = data[i];
    uint32_t w[80];
    for (int i = 0; i < 16; i++)
        w[i] = block[i * 4] | (block[i * 4 + 1] << 8) | (block[i * 4 + 2] << 16) | ((uint32_t)block[i * 4 + 3] << 24);
    for (int i = 16; i < 80; i++) {
        const unsigned amt = (w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16]) & 0x1F;
        w[i] = rotl32(1, amt);
    }
    uint32_t h0 = 0x67452301, h1 = 0xEFCDAB89, h2 = 0x98BADCFE, h3 = 0x10325476, h4 = 0xC3D2E1F0;
    uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
    for (int i = 0; i < 80; i++) {
        uint32_t f, k;
        if (i < 20) { f = (b & c) | (~b & d); k = 0x5A827999; }
        else if (i < 40) { f = b ^ c ^ d; k = 0x6ED9EBA1; }
        else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDC; }
        else { f = b ^ c ^ d; k = 0xCA62C1D6; }
        const uint32_t t = rotl32(a, 5) + f + e + k + w[i];
        e = d; d = c; c = rotl32(b, 30); b = a; a = t;
    }
    h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
    const uint32_t hs[5] = {h0, h1, h2, h3, h4};
    for (int j = 0; j < 5; j++) {
        out[j * 4] = (uint8_t)hs[j];
        out[j * 4 + 1] = (uint8_t)(hs[j] >> 8);
        out[j * 4 + 2] = (uint8_t)(hs[j] >> 16);
        out[j * 4 + 3] = (uint8_t)(hs[j] >> 24);
    }
}

// Standard FIPS SHA-1 -- used by CheckRevision (and D2's "SBig" CD-key hash), NOT the broken
// password variant above. Streams a small (< ~192 byte) message with proper 0x80/length padding,
// big-endian throughout.
void sha1(const uint8_t* data, int len, uint8_t out[20]) {
    uint32_t h0 = 0x67452301, h1 = 0xEFCDAB89, h2 = 0x98BADCFE, h3 = 0x10325476, h4 = 0xC3D2E1F0;
    const int total = (int)(((len + 8) / 64 + 1) * 64);
    uint8_t buf[256] = {0};
    for (int i = 0; i < len; i++) buf[i] = data[i];
    buf[len] = 0x80;
    const uint64_t bits = (uint64_t)len * 8;
    for (int i = 0; i < 8; i++) buf[total - 1 - i] = (uint8_t)(bits >> (8 * i));
    for (int off = 0; off < total; off += 64) {
        uint32_t w[80];
        for (int i = 0; i < 16; i++)
            w[i] = ((uint32_t)buf[off + i * 4] << 24) | (buf[off + i * 4 + 1] << 16) | (buf[off + i * 4 + 2] << 8) | buf[off + i * 4 + 3];
        for (int i = 16; i < 80; i++) w[i] = rotl32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
        for (int i = 0; i < 80; i++) {
            uint32_t f, k;
            if (i < 20) { f = (b & c) | (~b & d); k = 0x5A827999; }
            else if (i < 40) { f = b ^ c ^ d; k = 0x6ED9EBA1; }
            else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDC; }
            else { f = b ^ c ^ d; k = 0xCA62C1D6; }
            const uint32_t t = rotl32(a, 5) + f + e + k + w[i];
            e = d; d = c; c = rotl32(b, 30); b = a; a = t;
        }
        h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
    }
    const uint32_t hs[5] = {h0, h1, h2, h3, h4};
    for (int j = 0; j < 5; j++) {
        out[j * 4] = (uint8_t)(hs[j] >> 24);
        out[j * 4 + 1] = (uint8_t)(hs[j] >> 16);
        out[j * 4 + 2] = (uint8_t)(hs[j] >> 8);
        out[j * 4 + 3] = (uint8_t)hs[j];
    }
}
const char kB64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int b64Decode(const char* s, uint8_t* out) {
    int dec[256];
    for (int i = 0; i < 256; i++) dec[i] = -1;
    for (int i = 0; i < 64; i++) dec[(uint8_t)kB64[i]] = i;
    uint32_t acc = 0;
    int bits = 0, n = 0;
    for (const char* p = s; *p; p++) {
        if (*p == '=') break;
        const int v = dec[(uint8_t)*p];
        if (v < 0) continue;
        acc = (acc << 6) | (uint32_t)v;
        bits += 6;
        if (bits >= 8) { bits -= 8; out[n++] = (uint8_t)((acc >> bits) & 0xFF); }
    }
    return n;
}
int b64Encode(const uint8_t* d, int len, char* out) {
    int n = 0, i = 0;
    for (; i + 3 <= len; i += 3) {
        const uint32_t x = ((uint32_t)d[i] << 16) | (d[i + 1] << 8) | d[i + 2];
        out[n++] = kB64[(x >> 18) & 63]; out[n++] = kB64[(x >> 12) & 63]; out[n++] = kB64[(x >> 6) & 63]; out[n++] = kB64[x & 63];
    }
    const int rem = len - i;
    if (rem == 1) { const uint32_t x = (uint32_t)d[i] << 16; out[n++] = kB64[(x >> 18) & 63]; out[n++] = kB64[(x >> 12) & 63]; out[n++] = '='; out[n++] = '='; }
    else if (rem == 2) { const uint32_t x = ((uint32_t)d[i] << 16) | (d[i + 1] << 8); out[n++] = kB64[(x >> 18) & 63]; out[n++] = kB64[(x >> 12) & 63]; out[n++] = kB64[(x >> 6) & 63]; out[n++] = '='; }
    out[n] = 0;
    return n;
}
}  // namespace

enum : uint8_t {
    SID_NULL = 0x00,
    SID_PING = 0x25,
    SID_LOGONRESPONSE2 = 0x3A,
    SID_CREATEACCOUNT2 = 0x3D,
    SID_LOGONREALMEX = 0x3E,
    SID_QUERYREALMS2 = 0x40,
    SID_AUTH_INFO = 0x50,
    SID_AUTH_CHECK = 0x51,
};

extern "C" {

// BNCS frame header: [0xFF][id:u8][len:u16 LE], len counting the 4-byte header plus the body.
// Writes the 4 header bytes to `out`; returns the total frame length (4 + bodyLen), or -1 on
// overflow / a negative body.
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_header(uint8_t id, int bodyLen, uint8_t* out) {
    const int total = 4 + bodyLen;
    if (bodyLen < 0 || total > 0xFFFF) return -1;
    out[0] = 0xFF;
    out[1] = id;
    out[2] = (uint8_t)total;
    out[3] = (uint8_t)(total >> 8);
    return total;
}

// Decode the first four bytes of a BNCS frame: returns (total << 8) | id when buf[0] == 0xFF and at
// least 4 bytes are present, else -1.
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_decode(const uint8_t* buf, int len) {
    if (len < 4 || buf[0] != 0xFF) return -1;
    const int total = buf[2] | (buf[3] << 8);
    return (total << 8) | buf[1];
}

// Compose a SID_PING (0x25) echo: the 4-byte header plus the u32 cookie the server sent, verbatim.
// 8 bytes total.
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_ping(uint32_t cookie, uint8_t* out) {
    d2_realm_bncs_header(SID_PING, 4, out);
    out[4] = (uint8_t)cookie;
    out[5] = (uint8_t)(cookie >> 8);
    out[6] = (uint8_t)(cookie >> 16);
    out[7] = (uint8_t)(cookie >> 24);
    return 8;
}


// xSHA-1 of up to 64 bytes -> 20-byte digest.
EMSCRIPTEN_KEEPALIVE void d2_realm_bncs_xsha1(const uint8_t* data, int len, uint8_t* out20) {
    xsha1(data, len, out20);
}

// The stored OLS account hash: xSHA-1 of the password lowercased (the client lowercases before
// hashing, so a server that does not rejects the correct password for mixed-case accounts).
EMSCRIPTEN_KEEPALIVE void d2_realm_bncs_password_hash(const char* password, uint8_t* out20) {
    uint8_t lc[64];
    int n = 0;
    for (; password && password[n] && n < 64; n++) lc[n] = (uint8_t)tolower((unsigned char)password[n]);
    xsha1(lc, n, out20);
}

// SID_LOGONRESPONSE2 double-hash: xSHA-1(clientToken_le ++ serverToken_le ++ pwhash[20]).
EMSCRIPTEN_KEEPALIVE void d2_realm_bncs_double_hash(uint32_t clientToken, uint32_t serverToken, const uint8_t* pwhash20, uint8_t* out20) {
    uint8_t buf[28];
    buf[0] = (uint8_t)clientToken; buf[1] = (uint8_t)(clientToken >> 8); buf[2] = (uint8_t)(clientToken >> 16); buf[3] = (uint8_t)(clientToken >> 24);
    buf[4] = (uint8_t)serverToken; buf[5] = (uint8_t)(serverToken >> 8); buf[6] = (uint8_t)(serverToken >> 16); buf[7] = (uint8_t)(serverToken >> 24);
    memcpy(buf + 8, pwhash20, 20);
    xsha1(buf, 28, out20);
}

// The CheckRevision (D2 2020) response: base64(SHA1(first4(b64decode(challenge)) ++ ":" ++ version
// ++ ":" ++ sigOk)) -- standard SHA-1, not the broken variant. `challenge` is the server's base64
// version string, `version` is "a.b.c.d". Writes the base64 response to `out` and returns its
// length, or -1 if the decoded challenge is under 4 bytes.
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_checkrev(const char* challenge, const char* version, int sigOk, char* out) {
    uint8_t dec[256];
    const int dn = b64Decode(challenge, dec);
    if (dn < 4) return -1;
    uint8_t input[160];
    int il = 0;
    for (int i = 0; i < 4; i++) input[il++] = dec[i];
    input[il++] = ':';
    for (const char* v = version; *v; v++) input[il++] = (uint8_t)*v;
    input[il++] = ':';
    input[il++] = (uint8_t)sigOk;
    uint8_t digest[20];
    sha1(input, il, digest);
    return b64Encode(digest, 20, out);
}

// SID_AUTH_INFO (0x50) request body -- the version gauntlet's opening, before any account exists:
// nine u32s { 0, fourcc("IX86"), fourcc(product), 0x0E, 0, 0, 0, 0, 0 } then "USA\0United States\0"
// (d2-clientless realm.zig connect).  `fourcc` packs the first char into the HIGH byte, so writing
// it little-endian lands the tag reversed on the wire -- the classic BNCS convention.  The BNCS
// socket, like MCP's, opens with a single 0x01 protocol-selector byte before any frame.
// Returns the body length (54 for a 4-char product).
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_auth_info_request(const char* product, uint8_t* out) {
    const char* p = product && product[0] && product[1] && product[2] && product[3] ? product : "D2XP";
    const uint32_t ix86 = ((uint32_t)'I' << 24) | ((uint32_t)'X' << 16) | ((uint32_t)'8' << 8) | (uint32_t)'6';
    const uint32_t prod = ((uint32_t)(uint8_t)p[0] << 24) | ((uint32_t)(uint8_t)p[1] << 16) | ((uint32_t)(uint8_t)p[2] << 8) | (uint32_t)(uint8_t)p[3];
    const uint32_t words[9] = { 0, ix86, prod, 0x0E, 0, 0, 0, 0, 0 };
    int w = 0;
    for (int i = 0; i < 9; i++) {
        out[w++] = (uint8_t)words[i]; out[w++] = (uint8_t)(words[i] >> 8);
        out[w++] = (uint8_t)(words[i] >> 16); out[w++] = (uint8_t)(words[i] >> 24);
    }
    const char kLocale[] = "USA\0United States";
    for (int i = 0; i < (int)sizeof(kLocale) - 1; i++) out[w++] = (uint8_t)kLocale[i];
    out[w++] = 0;
    return w;
}

// SID_AUTH_CHECK (0x51) request body -- the answer: clientToken(u32), exeVersion(u32), exeHash(u32),
// keyCount(u32), spawn(u32=0), then `keyCount` 36-byte CD-key blocks, then the exe info string and
// the CD-key owner, both null-terminated (d2-clientless realm.zig).  Key blocks are passed through
// verbatim so the block's own crypto stays a separate concern; keyless (count 0) is the permissive
// path.  Returns the body length.
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_auth_check_request(uint32_t clientToken, uint32_t exeVersion, uint32_t exeHash, uint32_t keyCount, const uint8_t* keyBlocks, int keyBlocksLen, const char* exeInfo, const char* keyOwner, uint8_t* out) {
    const uint32_t head[5] = { clientToken, exeVersion, exeHash, keyCount, 0 };
    int w = 0;
    for (int i = 0; i < 5; i++) {
        out[w++] = (uint8_t)head[i]; out[w++] = (uint8_t)(head[i] >> 8);
        out[w++] = (uint8_t)(head[i] >> 16); out[w++] = (uint8_t)(head[i] >> 24);
    }
    for (int i = 0; i < keyBlocksLen; i++) out[w++] = keyBlocks[i];
    for (const char* q = exeInfo; q && *q; q++) out[w++] = (uint8_t)*q;
    out[w++] = 0;
    for (const char* q = keyOwner; q && *q; q++) out[w++] = (uint8_t)*q;
    out[w++] = 0;
    return w;
}

// ── The server side: the replies our realm sends (layouts from tristramd's bnetd) ───────────
namespace {
inline int wle32(uint8_t* p, uint32_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24); return 4; }
inline int wbe32(uint8_t* p, uint32_t v) { p[0] = (uint8_t)(v >> 24); p[1] = (uint8_t)(v >> 16); p[2] = (uint8_t)(v >> 8); p[3] = (uint8_t)v; return 4; }
inline int wcstr(uint8_t* p, const char* s) { int n = 0; for (; s && s[n]; n++) p[n] = (uint8_t)s[n]; p[n++] = 0; return n; }
}  // namespace

// SID_AUTH_INFO (0x50) reply: logonType(u32) serverToken(u32) udpValue(u32) mpqFiletime(u64), then
// the CheckRevision MPQ name and the challenge/formula, both null-terminated (tristramd
// sid_auth_info.c).  The 20-byte head is why the client reads the MPQ name at offset 20.
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_auth_info_reply(uint32_t logonType, uint32_t serverToken, uint32_t udpValue, uint32_t filetimeLo, uint32_t filetimeHi, const char* mpqName, const char* challenge, uint8_t* out) {
    int w = 0;
    w += wle32(out + w, logonType);
    w += wle32(out + w, serverToken);
    w += wle32(out + w, udpValue);
    w += wle32(out + w, filetimeLo);
    w += wle32(out + w, filetimeHi);
    w += wcstr(out + w, mpqName);
    w += wcstr(out + w, challenge);
    return w;
}

// SID_AUTH_CHECK (0x51) reply: result(u32) then an info string.  0x000 passed; 0x100/0x101 version
// problems, 0x200+ CD-key problems.
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_auth_check_reply(uint32_t result, const char* info, uint8_t* out) {
    int w = wle32(out, result);
    w += wcstr(out + w, info);
    return w;
}

// SID_LOGONRESPONSE2 (0x3a) reply: result(u32) -- 0 success, 1 no such account, 2 wrong password,
// 6 blocked (tristramd sid_logonresponse2.c).
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_logonresponse2_reply(uint32_t result, uint8_t* out) {
    return wle32(out, result);
}

// SID_QUERYREALMS2 (0x40) reply, one realm: unknown(u32=0) count(u32) then per realm unknown(u32=1),
// title and description null-terminated (tristramd sid_queryrealms2.c).
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_queryrealms2_reply(const char* title, const char* description, uint8_t* out) {
    int w = 0;
    w += wle32(out + w, 0);
    w += wle32(out + w, 1);
    w += wle32(out + w, 1);
    w += wcstr(out + w, title);
    w += wcstr(out + w, description);
    return w;
}

// SID_LOGONREALMEX (0x3e) success reply -- the MCP handoff: eighteen u32s then the unique name.
// cookie(clientToken), status, IP(big-endian), uuid, IP again, port in the high half (big-endian),
// then the block MCP_STARTUP forwards.  The product/platform tags here are chosen so that writing
// them LITTLE-endian spells them correctly on the wire ("D2XP", "IX86") -- the opposite of the
// AUTH_INFO request, where fourcc reverses them.  72 bytes before the name, which is exactly what
// MCP_STARTUP forwards: bytes [0,16) then [24,72).
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_logonrealmex_reply(uint32_t clientToken, uint32_t status, uint32_t ip, uint32_t uuid, uint16_t port, const char* uniqueName, uint8_t* out) {
    int w = 0;
    w += wle32(out + w, clientToken);
    w += wle32(out + w, status);
    w += wbe32(out + w, ip);
    w += wle32(out + w, uuid);
    w += wbe32(out + w, ip);
    w += wbe32(out + w, (uint32_t)port << 16);
    w += wle32(out + w, 1);
    w += wle32(out + w, 1);
    w += wle32(out + w, 1);
    w += wle32(out + w, 0x50583244u);   // spells "D2XP" little-endian on the wire
    w += wle32(out + w, 0x36385849u);   // spells "IX86" little-endian on the wire
    w += wle32(out + w, 1);
    w += wle32(out + w, 0x409);
    for (int i = 0; i < 5; i++) w += wle32(out + w, 1);
    w += wcstr(out + w, uniqueName);
    return w;
}

// SID_LOGONRESPONSE2 (0x3a) request body: clientToken(u32), serverToken(u32), the 20-byte login
// double-hash, then the account name null-terminated (d2-clientless realm.zig login).  The account
// password is hashed LOWERCASED (passwordHash).  Returns the body length (29 + account length).
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_logonresponse2_request(uint32_t clientToken, uint32_t serverToken, const char* account, const char* password, uint8_t* out) {
    uint8_t pw[20], dbl[20];
    d2_realm_bncs_password_hash(password, pw);
    d2_realm_bncs_double_hash(clientToken, serverToken, pw, dbl);
    out[0] = (uint8_t)clientToken; out[1] = (uint8_t)(clientToken >> 8); out[2] = (uint8_t)(clientToken >> 16); out[3] = (uint8_t)(clientToken >> 24);
    out[4] = (uint8_t)serverToken; out[5] = (uint8_t)(serverToken >> 8); out[6] = (uint8_t)(serverToken >> 16); out[7] = (uint8_t)(serverToken >> 24);
    for (int i = 0; i < 20; i++) out[8 + i] = dbl[i];
    int w = 28;
    for (const char* p = account; p && *p; p++) out[w++] = (uint8_t)*p;
    out[w++] = 0;
    return w;
}

// SID_LOGONREALMEX (0x3e) request body: clientToken(u32), the 20-byte realm double-hash, then the
// realm title null-terminated (d2-clientless realm.zig).  Unlike the account password, the realm
// password is hashed RAW -- plain xSHA-1, not lowercased.  Returns the body length (25 + title).
EMSCRIPTEN_KEEPALIVE int d2_realm_bncs_logonrealmex_request(uint32_t clientToken, uint32_t serverToken, const char* realmPassword, const char* realmTitle, uint8_t* out) {
    uint8_t raw[64], inner[20], dbl[20];
    int n = 0;
    for (; realmPassword && realmPassword[n] && n < 64; n++) raw[n] = (uint8_t)realmPassword[n];
    xsha1(raw, n, inner);
    d2_realm_bncs_double_hash(clientToken, serverToken, inner, dbl);
    out[0] = (uint8_t)clientToken; out[1] = (uint8_t)(clientToken >> 8); out[2] = (uint8_t)(clientToken >> 16); out[3] = (uint8_t)(clientToken >> 24);
    for (int i = 0; i < 20; i++) out[4 + i] = dbl[i];
    int w = 24;
    for (const char* p = realmTitle; p && *p; p++) out[w++] = (uint8_t)*p;
    out[w++] = 0;
    return w;
}

}  // extern "C"
