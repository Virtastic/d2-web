// 1.13c D2Client+0xAF8F0: ECX = packet. Copies its first 6 bytes (dword, word) into a stack local and calls
// 0x1E160 with ECX = (packet & 0xFFFF0000) | word (the `mov cx,[ecx+4]` keeps the pointer's high half) and the
// address of the copy as the one stack argument (callee pops it). 0x1E160 is injected.
#include <cstdint>
#include <cstring>

struct D2c0xaf8f0Env {
    void (*f1e160)(uint32_t ecx, const uint8_t* msg);
};

extern "C" void d2c_port_af8f0(const uint8_t* self, const D2c0xaf8f0Env& e) {
    uint8_t msg[8] = {0};
    std::memcpy(msg, self, 4);
    std::memcpy(msg + 4, self + 4, 2);
    uint16_t w;
    std::memcpy(&w, self + 4, 2);
    e.f1e160(((uint32_t)(uintptr_t)self & 0xFFFF0000u) | w, msg);
}
