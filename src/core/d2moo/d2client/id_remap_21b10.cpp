// 1.13c D2Client+0x21B10: remap a unit's id through the list at 0x11C4D4,
// then ask 0x4FD90 (mask 8) about it on the object at 0x11BBFC.
#include <cstdint>
#include <cstring>
namespace { uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; } }
// eax = unit (or 0); head = *0x11C4D4; g2 = *0x11BBFC; call4fd90(g2, id, 8) = D2Client+0x4FD90.
// The retail fatal-exit path (g2 == 0 or g2[+0xC] == -1) is not modelled: returns 0.
extern "C" uint32_t d2_client_21b10(uint32_t eax, uint32_t head, uint32_t g2, uint32_t (*call4fd90)(uint32_t, uint32_t, uint32_t)) {
    uint32_t key = eax ? rd(eax, 0xC) : 0xFFFFFFFFu, id = 0xFFFFFFFFu;
    for (uint32_t n = head; n; n = rd(n, 0x30))
        if (rd(n, 8) == key) { id = rd(n, 0xC); break; }
    if (!g2 || rd(g2, 0xC) == 0xFFFFFFFFu) return 0;
    return id != 0xFFFFFFFFu ? call4fd90(g2, id, 8) : 1;
}
