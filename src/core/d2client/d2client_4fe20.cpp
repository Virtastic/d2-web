// 1.13c D2Client+0x4FE20: push a key node (0x74 bytes: +0 key, +8 next) on the head of a record's key list unless the key is present.
// Registers: EAX = key, ECX = a, EDX = b (plain ret). Seams: the record lookup 0x9D630 (ECX/EDX kept),
// the key search 0x4FCE0 (ECX/EDX kept, ESI = key) and the allocator thunk 0xBF7A (ECX = 0xC, stack 0x74, 0).
// Addresses are 32-bit words in the probe's data model.
#include <cstdint>
#include <cstring>
namespace d2c4fe20 {
struct Env {
    void* c;
    uint32_t (*find)(void*, uint32_t a, uint32_t b);                // 0x9D630
    uint32_t (*has)(void*, uint32_t a, uint32_t b, uint32_t key);   // 0x4FCE0
    uint32_t (*alloc)(void*, uint32_t size, uint32_t zero);         // 0xBF7A
};
static uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
static void wr(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(p + o), &v, 4); }
// 1.13c D2Client+0x4FE20
void sub_4FE20(const Env& e, uint32_t key, uint32_t a, uint32_t b) {
    const uint32_t rec = e.find(e.c, a, b);
    if (!rec) return;
    const uint32_t head = rd(rec, 0x34);
    if (!head || b == key) return;
    if (e.has(e.c, a, b, key)) return;
    const uint32_t n = e.alloc(e.c, 0x74, 0);
    wr(n, 0, 0); wr(n, 4, 0); wr(n, 8, 0);
    wr(n, 0, key);
    wr(n, 8, rd(head, 0));
    wr(head, 0, n);
}
}
