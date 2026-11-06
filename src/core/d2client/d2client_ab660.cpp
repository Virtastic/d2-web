// 1.13c D2Client+0xAB660: walks the node list headed by global 0x11AD70 (next link, a 32-bit word at node+0x1F48,
// read before the call) and frees each node through Fog thunk 0xBF86 (ECX = node, EDX = source-file string,
// stack (0x120, 0), callee pops). The thunk is injected; the list head is passed in.
#include <cstdint>
#include <cstring>

struct D2c0xab660Env {
    void (*thunk_bf86)(const uint8_t* node, uint32_t line, uint32_t zero);
};

extern "C" void d2c_port_ab660(const uint8_t* head, const D2c0xab660Env& e) {
    const uint8_t* node = head;
    while (node) {
        uint32_t nx;
        std::memcpy(&nx, node + 0x1F48, 4);
        e.thunk_bf86(node, 0x120, 0);
        node = (const uint8_t*)(uintptr_t)nx;
    }
}
