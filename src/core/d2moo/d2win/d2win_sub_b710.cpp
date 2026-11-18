// 1.13c D2Win+0x0b710 (sub_B710): unlink a node (prev @+0xc8, next @+0xc4) from a doubly linked list.
// Stateful (global list head DAT_6f9a9e30, Fog #10043 seam); NAMED only, no vector.
#include <cstdint>
#include <cstring>

struct D2WinB710Env {
    uint32_t* head;                                        // DAT_6f9a9e30
    void (*free_mem)(int32_t line, int32_t zero);          // Fog #10043(0x84f, 0)
};

void d2win_sub_b710(uint8_t* node, const D2WinB710Env& env) {
    uint32_t prev, next;
    std::memcpy(&prev, node + 0xc8, 4);
    std::memcpy(&next, node + 0xc4, 4);
    uint8_t* P = (uint8_t*)(uintptr_t)prev;
    uint8_t* N = (uint8_t*)(uintptr_t)next;
    uint32_t zero = 0;
    if (!prev) {
        *env.head = next;
        if (next) std::memcpy(N + 0xc8, &zero, 4);
    } else if (!next) {
        std::memcpy(P + 0xc4, &zero, 4);
    } else {
        std::memcpy(P + 0xc4, &next, 4);
        std::memcpy(N + 0xc8, &prev, 4);
    }
    env.free_mem(0x84f, 0);
}
