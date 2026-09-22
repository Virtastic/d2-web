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
#include <netinet/in.h>
#include <netinet/tcp.h>

extern "C" {
void d2_realm_bncs_session_reset(uint32_t serverToken, uint32_t uuid, uint32_t ip, uint16_t port);
int d2_realm_bncs_session_feed(const uint8_t* in, int len, uint8_t* out, int outCap);
void d2_realm_mcp_session_reset(uint32_t gsIp, uint16_t gameToken, uint32_t gameHash, uint8_t charClass, uint8_t charLevel);
int d2_realm_mcp_session_feed(const uint8_t* in, int len, uint8_t* out, int outCap);
void d2_realm_d2gs_session_reset(uint32_t unitId, uint8_t charClass, uint8_t act, uint16_t level);
int d2_realm_d2gs_session_feed(const uint8_t* in, int len, uint8_t* out, int outCap);
int d2_realm_d2gs_session_greeting(uint8_t* out);
}

namespace {

enum Proto { kBncs = 0, kMcp = 1, kD2gs = 2, kProtoCount = 3 };
const char* kProtoName[kProtoCount] = { "BNCS", "MCP", "D2GS" };

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
        case kMcp:  d2_realm_mcp_session_reset(0x7f000001u, 0x1234, 0xdeadbeefu, 3, 24); break;
        case kD2gs: d2_realm_d2gs_session_reset(1, 3, 0, 24); break;
        default: break;
    }
}

}  // namespace

int main(int argc, char** argv) {
    uint16_t ports[kProtoCount] = { 6112, 6113, 4000 };
    for (int i = 1; i + 1 < argc; i += 2) {
        if (!strcmp(argv[i], "--bncs")) ports[kBncs] = (uint16_t)atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--mcp")) ports[kMcp] = (uint16_t)atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "--d2gs")) ports[kD2gs] = (uint16_t)atoi(argv[i + 1]);
    }

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
