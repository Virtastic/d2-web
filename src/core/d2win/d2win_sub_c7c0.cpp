// 1.13c D2Win+0xC7C0 (sub_C7C0): destroy every object on the global list (head DAT_6f9a9e30).
// Stateful (global list head, seams D2Win+0xC4A0 and Fog #10043); NAMED only, no vector.
#include <cstdint>
#include <cstring>

struct D2WinC7C0Env {
    uint32_t* head;                                  // DAT_6f9a9e30
    void (*teardown)(void* obj);                     // D2Win+0xC4A0
    void (*free_mem)(int32_t line, int32_t zero);    // Fog #10043(0xd2d, 0)
};

// 1.13c D2Win+0xC7C0: next link at +0xc4 is read before the teardown.
void d2win_sub_c7c0(const D2WinC7C0Env& env) {
    uint32_t p = *env.head;
    while (p) {
        uint32_t next; std::memcpy(&next, (uint8_t*)(uintptr_t)p + 0xc4, 4);
        env.teardown((void*)(uintptr_t)p);
        env.free_mem(0xd2d, 0);
        p = next;
    }
    *env.head = 0;
}
