#include "mcp_startup.h"
#include <cstring>

// Prior art: network protocol row; d2-clientless realm startup
// schema, cross-checked against the cached 1.13c bodies at 0x7510 and 0x6AD0.
// No D2MOO MCP implementation exists. Native imports deliberately stay external:
// a timer, an event or a socket is owned by the platform rather than fabricated.
D2McpStartupState d2_mcp_startup_state{};

// D2MCPClient+0x07510 (marker and composer: mcp_req_misc.cpp:d2_mcp_req_7510)
extern "C" uint32_t d2_mcp_req_7510(const uint8_t* reply, uint8_t* out);
extern "C" int32_t d2_mcp_startup_send(const uint8_t* realm_reply) {
    uint8_t packet[0x441];
    d2_mcp_startup_state.result = 0;
    const uint32_t total = d2_mcp_req_7510(realm_reply, packet);
    if (total == 0) d2_mcp_startup_overflow(0x26);
    return d2_mcp_fog_send(d2_mcp_startup_state.socket, packet, total);
}

// 1.13c D2MCPClient+0x06AD0
// Retail uses unsigned start+60000 <= now (including its wrap behaviour), polls
// the network before the event, and never clears a successful completion result.
extern "C" void d2_mcp_startup_wait(const uint8_t* realm_reply) {
    auto& state = d2_mcp_startup_state;
    if (state.event == nullptr) state.event = d2_mcp_os_create_event();
    if (d2_mcp_startup_send(realm_reply) == 0) {
        state.result = 11;
        return;
    }
    const uint32_t start = d2_mcp_os_tick_count();
    uint32_t scratch;
    for (;;) {
        if (d2_mcp_fog_socket_alive(state.socket) == 0 ||
            d2_mcp_fog_pump(&scratch) == 0 ||
            start + uint32_t{60000} <= d2_mcp_os_tick_count()) {
            state.result = 2;
            return;
        }
        const uint32_t wait = d2_mcp_os_wait_event(state.event, 0);
        if (wait == 0) return;
        if (wait != 0x102) {
            state.result = 2;
            return;
        }
        d2_mcp_os_sleep(10);
    }
}
