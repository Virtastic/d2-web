// 1.13c BNClient+0x0FD20 flush_queue. Under the critical section (0x131DC): while the
// queue head (0x1364C, next link at +0x128) is set: next := head->next, Fog #10043(head, "file", 0xA2, 0), head := next.
// The release is the node-carrying seam (retail passes the node in ECX); the port in bnclient_batch_b55a.cpp passes nullptr.
#include <cstdint>

namespace d2::bnclient::fd20 {
struct FlushEnv {
    void (*enter)(void* lock);
    void (*leave)(void* lock);
    void (*free_node)(void* node, uint32_t line);   // Fog #10043 via 0x80F0 (ECX = node, stack: line 0xA2, 0)
};

uint32_t flush_queue(const FlushEnv& env, void* lock, void** head) {
    env.enter(lock);
    while (*head) {
        void* cur = *head;
        void* next = *reinterpret_cast<void**>(static_cast<uint8_t*>(cur) + 0x128);
        env.free_node(cur, 0xA2);
        *head = next;
    }
    env.leave(lock);
    return 1;
}
}
