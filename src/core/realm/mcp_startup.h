#pragma once
#include <cstdint>

// One owner for the startup worker's module globals; handles are host pointers,
// while the result at retail +0xB1C8 remains a 32-bit value.
struct D2McpStartupState {
    void* socket;       // retail +0xB028
    void* event;        // retail +0xB218
    uint32_t result;    // retail +0xB1C8
};
extern D2McpStartupState d2_mcp_startup_state;
extern "C" {
int32_t d2_mcp_startup_send(const uint8_t* realm_reply);
void d2_mcp_startup_wait(const uint8_t* realm_reply);
// Platform/Fog import boundary. The Fog send consumes bytes synchronously.
void* d2_mcp_os_create_event(); // CreateEventA(nullptr,FALSE,FALSE,nullptr)
uint32_t d2_mcp_os_tick_count();
uint32_t d2_mcp_os_wait_event(void* event, uint32_t timeout);
void d2_mcp_os_sleep(uint32_t milliseconds);
int32_t d2_mcp_fog_socket_alive(void* socket); // Fog#10079
int32_t d2_mcp_fog_pump(uint32_t* scratch);    // Fog#10012
int32_t d2_mcp_fog_send(void* socket, const uint8_t* bytes, uint32_t size); // Fog#10070
[[noreturn]] void d2_mcp_startup_overflow(uint32_t message); // Fog#10265/#10024 then _exit(-1)
}
