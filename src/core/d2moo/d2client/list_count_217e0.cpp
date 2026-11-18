// 1.13c D2Client+0x217E0: count nodes of the list at D2Client+0x11C4D4 (next @+0x30) with node[+4]==type and node[+0xc]==(unit? unit[+0xc] : -1).
#include <cstdint>
#include <cstring>
static inline uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
extern "C" int32_t d2_client_217e0(uint32_t unit, uint32_t type, uint32_t head) {
    uint32_t key = unit ? rd(unit, 0xc) : 0xFFFFFFFFu; int32_t n = 0;
    for (uint32_t p = head; p; p = rd(p, 0x30)) if (rd(p, 4) == type && rd(p, 0xc) == key) n++;
    return n;
}
