// D2CMP SpriteCache teardown (1.13c 0xCE40). ESI = the cache struct; dwords hold native pointers (low 32 bits).
#include <cstdint>

// Cache dwords: [2] used, [3] count, [4] buffer, [5] aHead, [6] aTail, [7] bHead, [8] bTail, [9] freeHead,
// [10] freeTail, [11] destroy fn, [12] lock. Node dwords: [0] data, [1] size, [2] payload, [3] flags (byte, bit 1 = held),
// [4] key, [5] next, [6] prev. lookup = 1.13c BF40(key, cache); release = Ordinal_10046(file, line, 0);
// free_cs = Ordinal_10043(line, 0). The Enter/Leave/DeleteCriticalSection calls are the capture lock pin (no-ops).
static inline uint32_t* P(uint32_t v) { return (uint32_t*)(uintptr_t)v; }
static inline uint32_t V(const uint32_t* p) { return (uint32_t)(uintptr_t)p; }
static inline bool held(const uint32_t* n) { return (((const uint8_t*)n)[0xc] & 2) != 0; }

// 1.13c D2CMP+0xce40
extern "C" void d2cmp_ce40(uint32_t* st, uint32_t (*lookup)(uint32_t key, uint32_t* st), void (*destroy)(void),
                           void (*release)(uint32_t ptr, uint32_t line), void (*free_cs)(uint32_t line))
{
    if (st[3] != 0) {
        do {
            uint32_t* p = P(st[7]);
            if (p == nullptr) {
                p = P(st[5]);
                if (p == nullptr || held(p)) break;
                uint32_t n = p[5];
                st[5] = n;
                if (n == 0) st[6] = 0; else P(n)[6] = 0;
                p[5] = 0; p[6] = 0;
            } else if (st[5] == 0) {
                if (held(p)) break;
                uint32_t n = p[5];
                st[7] = n;
                if (n == 0) st[8] = 0; else P(n)[6] = 0;
                p[5] = 0; p[6] = 0;
            } else {
                p = P(lookup(p[4], st));
            }
            if (p == nullptr || held(p) || st[12] == 0) break;
            st[2] -= p[1];
            if (p[2] != 0) { destroy(); p[2] = 0; }
            if (p[0] != 0) { release(p[0], 0xa22); p[0] = 0; }
            if (st[10] == 0) st[9] = V(p);
            else { P(st[10])[5] = V(p); p[6] = st[10]; }
            st[10] = V(p);
            uint32_t left = st[3] - 1;
            st[3] = left;
            if (left == 0) break;
        } while (true);
    }
    if (st[4] != 0) { release(st[4], 0x94b); st[4] = 0; }
    if (st[12] != 0) { free_cs(0x95d); st[12] = 0; }
}
