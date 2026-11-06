// 1.13c D2Game+0xEBD10, injectable callees. Stack words: 0 = unit, 1 = item id; ret 8. Same chain walk as 0xEB8B0 (thunks 0xA3DC, 0xA46C,
// 0xA4CC, 1 word each). Result AX = the 1-based position of the node whose item has that unit id (-1 for a node without an item), else 0;
// the id -1 and a missing inventory give 0. Position 0x7FFF or more is a Fog assertion (retail 0xA2DA/0xA2C2/0x22A3C); the position is
// still returned if the exit hook returns. The upper half of EAX is residue (retmask 0xFFFF).
#include <cstdint>
struct GEBD10Env {
    uint32_t (*a3dc)(uint32_t inv);
    uint32_t (*a46c)(uint32_t node);
    uint32_t (*a4cc)(uint32_t node);
    void (*fatal)(uint32_t line);
};
// 1.13c D2Game+0xEBD10
uint16_t g_ebd10(uint32_t unit, uint32_t itemId, const GEBD10Env& env)
{
    if (itemId == 0xFFFFFFFFu) return 0;
    const uint32_t inv = *(uint32_t*)(uintptr_t)(unit + 0x60);
    if (inv == 0) return 0;
    uint32_t node = env.a3dc(inv);
    for (uint32_t pos = 1; node != 0; ++pos) {
        const uint32_t item = env.a46c(node);
        const uint32_t next = env.a4cc(node);
        const uint32_t id = item ? *(uint32_t*)(uintptr_t)(item + 0xC) : 0xFFFFFFFFu;
        if (id == itemId) {
            if (pos >= 0x7FFF) env.fatal(0x185);
            return (uint16_t)pos;
        }
        node = next;
    }
    return 0;
}
