// D2MCPClient 0x6BA0: the MCP connection thread. Opens the "clt->mcp" socket to the realm address the
// BNCS logon handed over, sends the one-byte protocol id, then runs the startup exchange (0x6AD0).
#include <cstdint>
#include <emscripten.h>
#include "../realm/mcp_startup.h"

// Inputs at retail +0xAEF0 (address), +0xAEF4 (port) and +0xAEE0 (the realm logon reply 0x6AD0 reads).
struct D2Mcp6ba0Inputs {
    uint32_t address;
    uint32_t port;
    const uint8_t* realm_reply;
};

struct D2Mcp6ba0Env {
    uint32_t (*resolve)(uint32_t address);                                   // Fog#10014 (ECX)
    void* (*open)(uint32_t addr, uint32_t port, const char* name);           // Fog#10068
    void (*set_buffer)(void* socket, uint32_t size, uint32_t flags);         // Fog#10077
    void (*start)(void* socket);                                             // Fog#10073
    void* (*connector)(void* socket);                                        // Fog#10079
    void (*connect)(void* connector, uint32_t* failed, uint32_t* aux);       // Fog#10016 (ECX, EDX, stack)
    void (*close)(void* socket);                                             // Fog#10075
    void (*send)(void* socket, const uint8_t* bytes, uint32_t size);         // Fog#10071
    void (*startup)(const uint8_t* realm_reply);                             // 0x6AD0
};

// 1.13c D2MCPClient+0x06BA0 (thread body, no arguments). result (+0xB1C8) is cleared, the socket is
// opened with a 0x5E80-byte buffer and started. No connector, or a nonzero failure flag from the
// connect, sets result 2 and returns 0. If the module socket (+0xB028) was replaced meanwhile, this
// socket is closed and 0 returned. Otherwise the protocol byte 1 is sent, the startup exchange runs,
// and the thread returns 1 exactly when result is still 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2mcpclient_conn_thread_6ba0(const D2Mcp6ba0Inputs* in,
                                                                      const D2Mcp6ba0Env* e) {
    D2McpStartupState& st = d2_mcp_startup_state;
    uint32_t addr = e->resolve(in->address);
    st.result = 0;
    void* h = e->open(addr, in->port, "clt->mcp");
    st.socket = h;
    e->set_buffer(h, 0x5E80, 0);
    e->start(h);
    uint32_t failed = 0, aux = 0;
    void* c = e->connector(h);
    if (!c) {
        st.result = 2;
        return 0;
    }
    e->connect(c, &failed, &aux);
    if (failed) {
        st.result = 2;
        return 0;
    }
    if (h != st.socket) {
        e->close(h);
        return 0;
    }
    const uint8_t proto = 1;
    e->send(h, &proto, 1);
    e->startup(in->realm_reply);
    return st.result == 0;
}
