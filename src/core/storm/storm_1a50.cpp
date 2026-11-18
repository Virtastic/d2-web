// 1.13c Storm+0x1A50 (30 B): pointer-table entry.
// Forwards its three stack words unchanged to 0x2210 with ESI = 1 and returns that callee's EAX. ret 0xC.
#include <cstdint>
#include <cstring>

struct Storm2210Seam { uint32_t (*call)(void* ctx, uint32_t esi, uint32_t a, uint32_t b, uint32_t c); void* ctx; };

uint32_t storm_1a50(const Storm2210Seam& sub, uint32_t a, uint32_t b, uint32_t c) {
    return sub.call(sub.ctx, 1, a, b, c);
}
