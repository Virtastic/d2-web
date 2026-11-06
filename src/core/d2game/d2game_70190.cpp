// 1.13c D2Game+0x70190, injectable callees. EAX = the inventory handed to thunk 0xA3DC (1 word). Counts the entries whose type (thunk 0xA6C4,
// low byte) is 3: for each node, 0xA46C (1 word) gives the item, 0xA4CC (1 word) the next node.
#include <cstdint>
struct G70190Env {
    uint32_t (*first)(uint32_t inv);    // 0xA3DC
    uint32_t (*item)(uint32_t node);    // 0xA46C
    uint32_t (*next)(uint32_t node);    // 0xA4CC
    uint32_t (*type)(uint32_t item);    // 0xA6C4 (result in AL)
};
// 1.13c D2Game+0x70190
int32_t g_70190(uint32_t eax, const G70190Env& env)
{
    int32_t n = 0;
    uint32_t node = env.first(eax);
    while (node != 0) {
        const uint32_t it = env.item(node);
        node = env.next(node);
        if ((env.type(it) & 0xFF) == 3) ++n;
    }
    return n;
}
