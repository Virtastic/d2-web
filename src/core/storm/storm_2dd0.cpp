// 1.13c Storm+0x2DD0: TSHashTable grow (stdcall, table on the stack). Retail 32-bit layout, words are u32 (a guest pointer is the low 32 bits):
// table +0x14 bucket array {cap, count, items, flag}, +0x1c items (12-byte buckets {linkoffset, term.prev, term.next}), +0x24 mask.
// Every node of the old buckets is moved (link at node+4) to a stack list, the bucket array is resized to 2 * (mask + 1) buckets through the
// Storm+0x32E0 seam, new or stale buckets are reset (offset 4, empty terminator), the mask is updated, and the nodes are redistributed by key & mask.
#include <cstdint>

typedef uint32_t u32;
struct S2DD0Env { void (*growBuckets)(u32* table, u32 newCount); };   // Storm+0x32E0 on the array at table+0x14

static inline u32* W(u32 a) { return (u32*)(uintptr_t)a; }
static inline u32 A(const void* p) { return (u32)(uintptr_t)p; }

static void unlinkLink(u32 l) {   // shared splice: l is the address of a {prev, next} link
    u32 prev = W(l)[0];
    if (!prev) return;
    u32 next = W(l)[1];
    u32 target = (int32_t)next < 0 ? ~next : next + (l - W(prev)[1]);
    *W(target) = prev;
    W(prev)[1] = next;
    W(l)[0] = 0; W(l)[1] = 0;
}

static void unlinkAll(u32* list) {   // Storm+0x2780 on {offset, prev, next}
    while ((int32_t)list[2] > 0) {
        u32 l = list[0] + list[2];
        if (!W(l)[0]) continue;   // retail: same loop, spins on an unlinked node (unreachable on valid lists)
        unlinkLink(l);
    }
}

static void append(u32* tail /* the term.prev word */, u32 link, u32 node) {
    u32 t = *tail;
    W(link)[0] = t;
    W(link)[1] = W(t)[1];
    W(t)[1] = node;
    *tail = link;
}

// 1.13c STORM+0x02DD0
extern "C" void Storm_Sub2DD0(const S2DD0Env& e, u32* table) {
    const u32 oldN = table[9] + 1, newN = oldN * 2;
    u32 tmp[3] = { 0xdddddddd, 0, 0 };   // stack list {offset, prev, next}
    tmp[1] = A(&tmp[1]); tmp[2] = ~A(&tmp[1]);
    unlinkAll(tmp); tmp[0] = 4; tmp[1] = A(&tmp[1]); tmp[2] = ~A(&tmp[1]);
    for (u32 i = 0; i < oldN; i++) {
        u32* b = W(table[7]) + i * 3;
        while ((int32_t)b[2] > 0) {
            u32 node = b[2], link = node + 4;
            unlinkLink(link);
            append(&tmp[1], link, node);
        }
    }
    e.growBuckets(table, newN);
    for (u32 i = 0; i < newN; i++) {
        u32* b = W(table[7]) + i * 3;
        if (b[0] != 4) {
            unlinkAll(b);
            b[1] = A(&b[1]); b[0] = 4; b[2] = ~A(&b[1]);
        }
    }
    table[9] = newN - 1;
    while ((int32_t)tmp[2] > 0) {
        u32 node = tmp[2];
        u32* b = W(table[7]) + (*W(node) & table[9]) * 3;
        u32 link = b[0] + node;
        unlinkLink(link);
        append(&b[1], link, node);
    }
    unlinkAll(tmp);
    if (tmp[1]) {   // the stack list's destructor splice
        u32 target = (int32_t)tmp[2] < 0 ? ~tmp[2] : tmp[2] + (A(&tmp[1]) - W(tmp[1])[1]);
        *W(target) = tmp[1];
        W(tmp[1])[1] = tmp[2];
    }
}
