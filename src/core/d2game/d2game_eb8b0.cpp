// 1.13c D2Game+0xEB8B0, injectable callees. EAX = unit, stack word 0 = 1-based index; ret 4. The inventory is the dword at unit+0x60;
// the node chain is walked with the D2Common thunks 0xA3DC (first node), 0xA46C (node to item) and 0xA4CC (next node), 1 word each.
// Result: the item's unit id (+0xC) at that position, or -1 (no inventory, chain too short, node without an item).
#include <cstdint>
struct GEB8B0Env {
    uint32_t (*a3dc)(uint32_t inv);
    uint32_t (*a46c)(uint32_t node);
    uint32_t (*a4cc)(uint32_t node);
};
// 1.13c D2Game+0xEB8B0
uint32_t g_eb8b0(uint32_t eax, uint32_t index, const GEB8B0Env& env)
{
    const uint32_t inv = *(uint32_t*)(uintptr_t)(eax + 0x60);
    if (inv == 0) return 0xFFFFFFFFu;
    uint32_t node = env.a3dc(inv);
    for (uint32_t pos = 1; node != 0; ++pos) {
        const uint32_t item = env.a46c(node);
        const uint32_t next = env.a4cc(node);
        if (pos == index) return item ? *(uint32_t*)(uintptr_t)(item + 0xC) : 0xFFFFFFFFu;
        node = next;
    }
    return 0xFFFFFFFFu;
}
