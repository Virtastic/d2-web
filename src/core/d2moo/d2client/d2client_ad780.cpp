// 1.13c D2Client+0xAD780: ECX = packet. rec = 0x9D630(EDX = dword at packet+1); when non-null: rec+0x22 := word at
// packet+5, 0x9DA70(), then tail-jumps 0x772E0. The three callees are injected.
#include <cstdint>
#include <cstring>

struct D2c0xad780Env {
    uint8_t* (*f9d630)(uint32_t edx);
    void (*f9da70)();
    void (*f772e0)();
};

extern "C" void d2c_port_ad780(const uint8_t* pkt, const D2c0xad780Env& e) {
    uint32_t id;
    std::memcpy(&id, pkt + 1, 4);
    uint8_t* rec = e.f9d630(id);
    if (!rec) return;
    std::memcpy(rec + 0x22, pkt + 5, 2);
    e.f9da70();
    e.f772e0();
}
