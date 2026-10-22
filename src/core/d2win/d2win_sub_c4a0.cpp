// 1.13c D2Win+0xC4A0 (sub_C4A0): tear down the list/buffer owned by a control-like object.
// Alpha batch lane: implementation + marker only. Callees are seams (Fog #10265/#10024/#10043/#10065
// and D2Win+0xBA00 / +0xB710 are not implemented in src/).
#include <cstdint>
#include <cstring>
#include <unistd.h>

struct D2WinC4A0Env {
    void (*release_node)(void* node, int32_t freeData);   // D2Win+0xBA00 (ESI = node, stack = 1)
    void (*free_mem)(void* p, int32_t line);              // Fog #10043(line, 0)
    void (*query)(void* p);                               // Fog #10065
    void (*tail)(void* obj);                              // D2Win+0xB710
};

// 1.13c D2Win+0xC4A0. `unaff_retaddr` in the decompile is the caller's EBP-frame word; callers pass
// it as `tailFlag` (nonzero -> run 0xB710). obj is a low-heap pointer.
void d2win_sub_c4a0(uint8_t* obj, int32_t tailFlag, const D2WinC4A0Env& env) {
    if (!obj) _exit(-1);                                  // Fog #10265(0x86c) -> #10024 -> _exit
    uint32_t* head = nullptr;
    uint32_t h; std::memcpy(&h, obj + 0xb8, 4);
    head = (uint32_t*)(uintptr_t)h;
    if (!head) _exit(-1);                                 // 0x870
    uint32_t node = *head;
    while (node) {
        uint32_t next; std::memcpy(&next, (uint8_t*)(uintptr_t)node + 4, 4);
        env.release_node((void*)(uintptr_t)node, 1);
        env.free_mem((void*)(uintptr_t)node, 0x878);
        node = next;
    }
    *head = 0;
    env.free_mem(head, 0x87e);
    uint32_t buf; std::memcpy(&buf, obj + 0xbc, 4);
    if (buf) {
        env.query((void*)(uintptr_t)buf);
        env.free_mem((void*)(uintptr_t)buf, 0x5c);
        uint32_t z = 0; std::memcpy(obj + 0xbc, &z, 4);
    }
    if (tailFlag) env.tail(obj);
}
