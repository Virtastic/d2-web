// 1.13c D2Win+0xBA00 (sub_BA00): drop one reference on a node of the global doubly linked list
// (head DAT_6f9a9e34); at zero, unlink it. Stateful (global head, ESI = node, stack = freeData).
#include <cstdint>
#include <cstring>
#include <unistd.h>

struct D2WinBA00Env {
    uint32_t* head;                                    // DAT_6f9a9e34
    void (*free_mem)(int32_t line, int32_t zero);      // Fog #10043
};

// node layout: +0 refcount, +0x14 data, +0x18 next, +0x1c prev
void d2win_sub_ba00(uint8_t* node, int32_t freeData, D2WinBA00Env& env) {
    if (!node) _exit(-1);                              // Fog #10265(0x44a) -> #10024 -> _exit
    if (!*env.head) return;
    int32_t rc; std::memcpy(&rc, node, 4); rc--; std::memcpy(node, &rc, 4);
    if (rc >= 1) return;
    uint32_t next, prev; std::memcpy(&next, node + 0x18, 4); std::memcpy(&prev, node + 0x1c, 4);
    if (freeData) { env.free_mem(0x456, 0); uint32_t z = 0; std::memcpy(node + 0x14, &z, 4); }
    uint8_t* n = (uint8_t*)(uintptr_t)next; uint8_t* p = (uint8_t*)(uintptr_t)prev;
    if (!prev) { *env.head = next; if (n) std::memset(n + 0x1c, 0, 4); }
    else if (!next) std::memset(p + 0x18, 0, 4);
    else { std::memcpy(p + 0x18, &next, 4); std::memcpy(n + 0x1c, &prev, 4); }
    env.free_mem(0x46f, 0);
}
