// EAX = unit a, ECX = unit b. a's id (a player's is mapped
// through the module list 0x11C4D4: node +8 key, +0xC value, +0x30 next; no node or value -1 gives 0) and b's id are
// each looked up with 0x9D630 (head 0x11BC14); 1 when both entries exist and share a word +0x22 that is not 0xFFFF.
#include <cstdint>
#include "cl_env.h"
typedef uint32_t (*d2c_rd32_fn)(uint32_t addr);
uint32_t d2c_9d630(d2c_rd32_fn rd, uint32_t head, int32_t id);
namespace { constexpr uint32_t kBase = 0x6FAB0000u; }
// 1.13c D2Client+0xA0EE0
extern "C" uint32_t d2c_a0ee0(uint32_t a, uint32_t b, const cl::Env& e, d2c_rd32_fn rd) {
    int32_t k = -1;
    if (a) {
        k = (int32_t)e.R(a + 0xc);
        if (e.R(a) == 1) {
            uint32_t n = e.R(kBase + 0x11c4d4);
            while (n && (int32_t)e.R(n + 8) != k) n = e.R(n + 0x30);
            if (!n) return 0;
            k = (int32_t)e.R(n + 0xc);
            if (k == -1) return 0;
        }
    }
    const uint32_t head = e.R(kBase + 0x11bc14);
    const uint32_t x = d2c_9d630(rd, head, b ? (int32_t)e.R(b + 0xc) : -1);
    const uint32_t y = d2c_9d630(rd, head, k);
    if (!x || !y) return 0;
    const uint32_t w = e.R(x + 0x22) & 0xffff;
    return w == (e.R(y + 0x22) & 0xffff) && w != 0xffff ? 1u : 0u;
}
