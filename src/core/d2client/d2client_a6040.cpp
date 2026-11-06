// 1.13c D2Client+0xA6040: ECX = key, EDX = type, stack word 0 = value (ret 4). Looks the entity up with 0xA4E20
// (EAX = key & 0x7F, EDX = row base = bucket table + type * 0x200, ECX = key, stack word = type; ret 4), then
// calls D2Common Ordinal 11129 (thunk 0xC62E, two stack words popped by the callee) with (value, node).
// 0xA4E20 and the thunk are injected; the bucket table base is the module global 0x11A608 (an address, passed in).
#include <cstdint>

struct D2c0xa6040Env {
    uint32_t (*fa4e20)(uint32_t eax, uint32_t edx, uint32_t ecx, uint32_t type);
    void (*thunk_c62e)(uint32_t value, uint32_t node);
};

extern "C" void d2c_port_a6040(uint32_t key, uint32_t type, uint32_t value, uint32_t bucketBase, const D2c0xa6040Env& e) {
    uint32_t node = e.fa4e20(key & 0x7F, bucketBase + (type << 9), key, type);
    e.thunk_c62e(value, node);
}
