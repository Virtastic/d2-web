// 1.13c D2Client+0xA0E70: ECX = unit (the stack word is unused, ret 4). Returns 0 when 0xA0A30(EAX = unit) is 0,
// otherwise D2Common Ordinal 10641 (thunk 0xC6EE, two stack words popped by the callee) of (unit, 0) != 0.
#include <cstdint>

struct D2c0xa0e70Env {
    uint32_t (*fa0a30)(uint32_t eax);
    uint32_t (*thunk_c6ee)(uint32_t unit, uint32_t zero);
};

extern "C" uint32_t d2c_port_a0e70(uint32_t unit, const D2c0xa0e70Env& e) {
    if (e.fa0a30(unit) == 0) return 0;
    return e.thunk_c6ee(unit, 0) != 0 ? 1u : 0u;
}
