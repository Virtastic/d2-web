// realmd -- the native host that puts the realm sessions on a real socket.
//
// Everything protocol-shaped lives in src/core/realm and is pure: bytes in, bytes out, no I/O. This
// file is the only part that touches a socket, and it is deliberately the thinnest thing that can
// work -- accept, read, feed, write. The point is that the SAME sources the wasm engine compiles
// serve a stock client here, so there is no second copy of a codec to drift.
//
// Connections are handled CONCURRENTLY, one per protocol. That is not a nicety: a real client
// keeps its BNCS chat socket open while it dials MCP and then D2GS, so a server that served one
// connection to completion before accepting the next would accept BNCS, block forever reading it,
// and never accept the MCP connection at all -- the client would simply hang at the realm list.
// An earlier version of this file did exactly that.
//
// Honest limitation that remains: each protocol has a single session instance (the engine keeps its
// state in one place, as the rest of this project does), so realmd serves one connection PER
// PROTOCOL -- enough for one player through the whole handshake and into a game. Concurrent players
// need the session state made per-connection first.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

extern "C" {
void d2_realm_bncs_session_reset(uint32_t serverToken, uint32_t uuid, uint32_t ip, uint16_t port);
int d2_realm_bncs_session_feed(const uint8_t* in, int len, uint8_t* out, int outCap);
void d2_realm_mcp_session_reset(uint32_t gsIp, uint16_t gameToken, uint32_t gameHash, uint8_t charClass, uint8_t charLevel);
int d2_realm_mcp_session_feed(const uint8_t* in, int len, uint8_t* out, int outCap);
void d2_realm_d2gs_session_reset(uint32_t unitId, uint8_t charClass, uint8_t act, uint16_t level,
                                  uint16_t gameToken, uint32_t gameHash);
#if defined(D2_REALMD_TEST_FIXTURE)
void d2_realm_d2gs_session_fixture_reassign_on_keepalive(int enabled);
void d2_realm_d2gs_session_fixture_reassign_on_walk(int enabled);
void d2_realm_d2gs_session_fixture_monster_on_join(int enabled);
void d2_realm_d2gs_session_fixture_remove_monster_on_walk(int enabled);
void d2_realm_d2gs_session_fixture_other_player_on_join(int enabled);
void d2_realm_d2gs_session_fixture_select_other_on_keepalive(int enabled);
void d2_realm_d2gs_session_fixture_return_self_remove_other_on_walk(int enabled);
#endif
int d2_realm_d2gs_session_feed(const uint8_t* in, int len, uint8_t* out, int outCap);
int d2_realm_d2gs_session_greeting(uint8_t* out);
#if defined(D2_REALMD_TEST_FIXTURE)
int d2_realm_d2gs_cs_packet_len(const uint8_t* in, int len);
int d2_realm_huffman_compress(const uint8_t* in, int len, uint8_t* out, int outCap);
int d2_realm_huffman_frame(const uint8_t* in, int len, uint8_t* out, int outCap);
#endif
}

namespace {

struct SessionConfig {
    uint32_t gsIp = 0x7f000001u;
    uint16_t gameToken = 0x1234;
    uint32_t gameHash = 0xdeadbeefu;
    uint32_t unitId = 1;
    uint8_t charClass = 3;
    uint8_t charLevel = 24;
};

SessionConfig sessionConfig;

enum Proto { kBncs = 0, kMcp = 1, kD2gs = 2, kProtoCount = 3 };
const char* kProtoName[kProtoCount] = { "BNCS", "MCP", "D2GS" };
#if defined(D2_REALMD_TEST_FIXTURE)
bool fixtureFireballOnJoin = false;
bool fixtureFireballSent = false;
bool fixtureOutdoorOnWalk = false;
bool fixtureOutdoorSent = false;
uint8_t fixtureInput[512]{};
int fixtureInputLength = 0;

int appendD2gsFixtureFrame(const uint8_t* payload, int length, uint8_t* out, int outLength, int outCap) {
    uint8_t compressed[256], framed[320];
    const int compressedLength = d2_realm_huffman_compress(payload, length, compressed, sizeof compressed);
    if (compressedLength < 0) return -1;
    const int framedLength = d2_realm_huffman_frame(compressed, compressedLength, framed, sizeof framed);
    if (framedLength < 0 || outLength + framedLength > outCap) return -1;
    std::memcpy(out + outLength, framed, static_cast<size_t>(framedLength));
    return outLength + framedLength;
}

int applyD2gsFixtureReplies(const uint8_t* input, int length, uint8_t* out, int outLength, int outCap) {
    if (length < 0 || length > int(sizeof fixtureInput) - fixtureInputLength) return -1;
    std::memcpy(fixtureInput + fixtureInputLength, input, static_cast<size_t>(length));
    fixtureInputLength += length;
    int consumed = 0;
    for (;;) {
        const int packetLength = d2_realm_d2gs_cs_packet_len(fixtureInput + consumed, fixtureInputLength - consumed);
        if (packetLength < 0) break;
        if (!packetLength) return -1;
        const uint8_t* packet = fixtureInput + consumed;
        if (packet[0] == 0x68 && fixtureFireballOnJoin && !fixtureFireballSent) {
            // Fixture-only server packets: learned prerequisites, learned Fireball, active right
            // hand, and enough current mana. They are compressed with the production D2GS codec.
            const uint8_t skills[12] = { 0x94, 2, 1, 0, 0, 0, 36, 0, 1, 47, 0, 3 };
            const uint8_t active[13] = { 0x23, 0, 1, 0, 0, 0, 0, 47, 0, 0xff, 0xff, 0xff, 0xff };
            const uint8_t mana[6] = { 0x1f, 8, 0x00, 0x64, 0x00, 0x00 };
            outLength = appendD2gsFixtureFrame(skills, sizeof skills, out, outLength, outCap);
            if (outLength < 0) return -1;
            outLength = appendD2gsFixtureFrame(active, sizeof active, out, outLength, outCap);
            if (outLength < 0) return -1;
            outLength = appendD2gsFixtureFrame(mana, sizeof mana, out, outLength, outCap);
            if (outLength < 0) return -1;
            fixtureFireballSent = true;
        }
        if (packet[0] == 0x01 && fixtureOutdoorOnWalk && !fixtureOutdoorSent && packetLength >= 5 &&
            (packet[1] | (uint16_t(packet[2]) << 8)) == 5860 &&
            (packet[3] | (uint16_t(packet[4]) << 8)) == 5894) {
            // The ordinary walk command gets a server-side MapReveal and player location update
            // into seed-1 Blood Moor, where the subsequent local cast can use its native outdoor gate.
            const uint8_t outdoor[17] = { 0x07, 0x58, 0x04, 0xa0, 0x04, 2,
                0x15, 0, 1, 0, 0, 0, 0xba, 0x15, 0x22, 0x17, 0x5a };
            outLength = appendD2gsFixtureFrame(outdoor, sizeof outdoor, out, outLength, outCap);
            if (outLength < 0) return -1;
            fixtureOutdoorSent = true;
        }
        consumed += packetLength;
        if (consumed == fixtureInputLength) break;
    }
    if (consumed) {
        fixtureInputLength -= consumed;
        std::memmove(fixtureInput, fixtureInput + consumed, static_cast<size_t>(fixtureInputLength));
    }
    return outLength;
}
#endif

int listenOn(uint16_t port) {
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    int one = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    a.sin_port = htons(port);
    if (bind(fd, (sockaddr*)&a, sizeof a) < 0) { close(fd); return -1; }
    if (listen(fd, 4) < 0) { close(fd); return -1; }
    return fd;
}

bool writeAll(int fd, const uint8_t* p, int n) {
    while (n > 0) {
        const ssize_t w = write(fd, p, (size_t)n);
        if (w <= 0) { if (errno == EINTR) continue; return false; }
        p += w;
        n -= (int)w;
    }
    return true;
}

// Handle whatever is readable on one connection. Returns false when it should be closed.
bool pump(int fd, Proto proto) {
    uint8_t in[4096];
    uint8_t out[16384];
    const ssize_t r = read(fd, in, sizeof in);
    if (r <= 0) return false;   // client closed, or error
    int n = 0;
    switch (proto) {
        case kBncs: n = d2_realm_bncs_session_feed(in, (int)r, out, (int)sizeof out); break;
        case kMcp:  n = d2_realm_mcp_session_feed(in, (int)r, out, (int)sizeof out); break;
        case kD2gs: n = d2_realm_d2gs_session_feed(in, (int)r, out, (int)sizeof out); break;
        default: return false;
    }
#if defined(D2_REALMD_TEST_FIXTURE)
    if (n >= 0 && proto == kD2gs) {
        n = applyD2gsFixtureReplies(in, static_cast<int>(r), out, n, static_cast<int>(sizeof out));
    }
#endif
    if (n < 0) {
        fprintf(stderr, "[%s] framing failed; dropping the connection rather than desyncing\n", kProtoName[proto]);
        return false;
    }
    if (n > 0 && !writeAll(fd, out, n)) return false;
    return true;
}

void resetFor(Proto proto) {
    switch (proto) {
        case kBncs: d2_realm_bncs_session_reset(0x1234abcdu, 0xabcd1234u, 0x7f000001u, 6113); break;
        case kMcp:
#if defined(D2_REALMD_TEST_FIXTURE)
            d2_realm_mcp_session_reset(sessionConfig.gsIp, sessionConfig.gameToken, sessionConfig.gameHash,
                                       sessionConfig.charClass, sessionConfig.charLevel);
#else
            d2_realm_mcp_session_reset(sessionConfig.gsIp, sessionConfig.gameToken, sessionConfig.gameHash,
                                       sessionConfig.charClass, sessionConfig.charLevel);
#endif
            break;
        case kD2gs:
#if defined(D2_REALMD_TEST_FIXTURE)
            fixtureInputLength = 0;
            fixtureFireballSent = false;
            fixtureOutdoorSent = false;
            d2_realm_d2gs_session_reset(sessionConfig.unitId, sessionConfig.charClass, 0, sessionConfig.charLevel,
                                        sessionConfig.gameToken, sessionConfig.gameHash);
#else
            d2_realm_d2gs_session_reset(sessionConfig.unitId, sessionConfig.charClass, 0, sessionConfig.charLevel,
                                        sessionConfig.gameToken, sessionConfig.gameHash);
#endif
            break;
        default: break;
    }
}

}  // namespace

int main(int argc, char** argv) {
    uint16_t ports[kProtoCount] = { 6112, 6113, 4000 };
#if defined(D2_REALMD_TEST_FIXTURE)
    int fixtureReassignOnKeepalive = 0;
    int fixtureReassignOnWalk = 0;
    int fixtureMonsterOnJoin = 0;
    int fixtureRemoveMonsterOnWalk = 0;
    int fixtureOtherPlayerOnJoin = 0;
    int fixtureSelectOtherOnKeepalive = 0;
    int fixtureReturnSelfRemoveOtherOnWalk = 0;
    int fixtureFireball = 0;
    int fixtureOutdoor = 0;
#endif
    for (int i = 1; i + 1 < argc; i += 2) {
        if (!strcmp(argv[i], "--bncs")) ports[kBncs] = (uint16_t)atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--mcp")) ports[kMcp] = (uint16_t)atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--d2gs")) ports[kD2gs] = (uint16_t)atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--game-token")) sessionConfig.gameToken = (uint16_t)strtoul(argv[i + 1], nullptr, 0);
        else if (!strcmp(argv[i], "--game-hash")) sessionConfig.gameHash = (uint32_t)strtoul(argv[i + 1], nullptr, 0);
        else if (!strcmp(argv[i], "--unit-id")) sessionConfig.unitId = (uint32_t)strtoul(argv[i + 1], nullptr, 0);
        else if (!strcmp(argv[i], "--char-class")) sessionConfig.charClass = (uint8_t)strtoul(argv[i + 1], nullptr, 0);
        else if (!strcmp(argv[i], "--char-level")) sessionConfig.charLevel = (uint8_t)strtoul(argv[i + 1], nullptr, 0);
        else if (!strcmp(argv[i], "--gs-ip")) {
            in_addr address{};
            if (inet_pton(AF_INET, argv[i + 1], &address) != 1) {
                fprintf(stderr, "realmd: invalid IPv4 address for --gs-ip: %s\n", argv[i + 1]);
                return 2;
            }
            sessionConfig.gsIp = ntohl(address.s_addr);
        }
#if defined(D2_REALMD_TEST_FIXTURE)
        else if (!strcmp(argv[i], "--fixture-reassign-on-keepalive")) fixtureReassignOnKeepalive = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--fixture-reassign-on-walk")) fixtureReassignOnWalk = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--fixture-monster-on-join")) fixtureMonsterOnJoin = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--fixture-remove-monster-on-walk")) fixtureRemoveMonsterOnWalk = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--fixture-other-player-on-join")) fixtureOtherPlayerOnJoin = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--fixture-select-other-on-keepalive")) fixtureSelectOtherOnKeepalive = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--fixture-return-self-remove-other-on-walk")) fixtureReturnSelfRemoveOtherOnWalk = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--fixture-fireball-on-join")) fixtureFireball = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--fixture-outdoor-on-walk")) fixtureOutdoor = atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--fixture-char-class")) sessionConfig.charClass = (uint8_t)atoi(argv[i + 1]);
#endif
    }
#if defined(D2_REALMD_TEST_FIXTURE)
    d2_realm_d2gs_session_fixture_reassign_on_keepalive(fixtureReassignOnKeepalive);
    d2_realm_d2gs_session_fixture_reassign_on_walk(fixtureReassignOnWalk);
    d2_realm_d2gs_session_fixture_monster_on_join(fixtureMonsterOnJoin);
    d2_realm_d2gs_session_fixture_remove_monster_on_walk(fixtureRemoveMonsterOnWalk);
    d2_realm_d2gs_session_fixture_other_player_on_join(fixtureOtherPlayerOnJoin);
    d2_realm_d2gs_session_fixture_select_other_on_keepalive(fixtureSelectOtherOnKeepalive);
    d2_realm_d2gs_session_fixture_return_self_remove_other_on_walk(fixtureReturnSelfRemoveOtherOnWalk);
    fixtureFireballOnJoin = fixtureFireball != 0;
    fixtureOutdoorOnWalk = fixtureOutdoor != 0;
#endif

    int lfd[kProtoCount];
    for (int p = 0; p < kProtoCount; p++) {
        lfd[p] = listenOn(ports[p]);
        if (lfd[p] < 0) { fprintf(stderr, "realmd: cannot listen on %u\n", ports[p]); return 1; }
    }
    // Announce readiness on stdout so a supervisor can wait for it rather than sleep.
    printf("realmd listening bncs=%u mcp=%u d2gs=%u\n", ports[kBncs], ports[kMcp], ports[kD2gs]);
    fflush(stdout);

    int conn[kProtoCount] = { -1, -1, -1 };
    while (true) {
        fd_set rs;
        FD_ZERO(&rs);
        int mx = 0;
        for (int p = 0; p < kProtoCount; p++) {
            FD_SET(lfd[p], &rs);
            if (lfd[p] > mx) mx = lfd[p];
            if (conn[p] >= 0) { FD_SET(conn[p], &rs); if (conn[p] > mx) mx = conn[p]; }
        }
        if (select(mx + 1, &rs, nullptr, nullptr, nullptr) < 0) { if (errno == EINTR) continue; break; }

        // Existing connections first, so a client that is mid-handshake is not starved by a new
        // accept on another port.
        for (int p = 0; p < kProtoCount; p++) {
            if (conn[p] < 0 || !FD_ISSET(conn[p], &rs)) continue;
            if (!pump(conn[p], (Proto)p)) { close(conn[p]); conn[p] = -1; }
        }
        for (int p = 0; p < kProtoCount; p++) {
            if (!FD_ISSET(lfd[p], &rs)) continue;
            const int fd = accept(lfd[p], nullptr, nullptr);
            if (fd < 0) continue;
            // One session instance per protocol: a second connection replaces the first rather
            // than quietly sharing its state.
            if (conn[p] >= 0) { close(conn[p]); }
            int one = 1;
            setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof one);
            resetFor((Proto)p);
            conn[p] = fd;
            if ((Proto)p == kD2gs) {
                // The engine greets unprompted, before the client says anything.
                uint8_t g[8];
                const int n = d2_realm_d2gs_session_greeting(g);
                if (!writeAll(fd, g, n)) { close(fd); conn[p] = -1; }
            }
        }
    }
    for (int p = 0; p < kProtoCount; p++) if (conn[p] >= 0) close(conn[p]);
    for (int p = 0; p < kProtoCount; p++) close(lfd[p]);
    return 0;
}
