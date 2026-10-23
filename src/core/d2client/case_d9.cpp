// 1.13c D2Client+0x258F0: throttled distance check.
#include <cstdint>
// Returns the low-byte result (1 keep, 0 removed). Side effects injected by the caller.
// *last = p+0xD4. dist() is Ordinal_10522(p, target). remove() is 0xA6EF0(table, id&0x7f, id, 2).
template <class Dist, class Remove>
uint8_t D2Client_CaseD9(uint32_t now, uint32_t* last, uint32_t id, Dist dist, Remove remove) {
    if (*last != 0) {
        if (now - *last <= 1000) return 1;
        if (dist() > 25) { remove(id & 0x7f, id, 2); return 0; }
    }
    *last = now;
    return 1;
}
