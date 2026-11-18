// 1.13c Storm+0x21B10 (30 B): pointer-table entry.
// Calls 0x21560 with EAX = the incoming ECX, ECX = the second stack word, and three stack words: the dword at [EDX], the first stack word, and the constant 2. Stores the callee's EAX at [EDX] and returns it. ret 8. The sibling entry of the same table differs only in the constant.
#include <cstdint>
#include <cstring>

struct Storm21560Seam { uint32_t (*call)(void* ctx, uint32_t eax, uint32_t ecx, uint32_t cur, uint32_t s0, uint32_t mode); void* ctx; };

uint32_t storm_21b10(const Storm21560Seam& sub, uint32_t a, uint32_t* slot, uint32_t s0, uint32_t s1) {
    const uint32_t v = sub.call(sub.ctx, a, s1, *slot, s0, 2);
    *slot = v;
    return v;
}
