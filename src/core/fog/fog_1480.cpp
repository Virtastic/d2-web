// Prior art used: none; the control flow was read from the 1.13c decompile cache (Fog+0x01480, 159 bytes).
// Callees: Fog+0x1790 (granule default; the heap is its one stack argument, it returns the granule) and
// Fog+0x17D0 (array resize; the new capacity is its one stack argument).
#include <cstdint>

// Heap object (retail layout): +0 cap, +4 count, +8 slots (u32 node offsets), +0xC granule (0 = default), +0x10 base.
// A node sits at base + slot value: +0 its vtable pointer, +4 the heap, +8 its index in the slots.
struct Fog1480Heap {
    uint32_t cap;
    uint32_t count;
    uint32_t slots;
    uint32_t granule;
    uint32_t base;
};

typedef uint32_t (*Fog1480Granule)(Fog1480Heap* h);   // 1.13c Fog+0x1790
typedef void (*Fog1480Resize)(uint32_t newcap);        // 1.13c Fog+0x17D0
// Calls the function at vtable slot 0 (fn) with one stack argument (arg); nonzero stops the sift.
typedef int32_t (*Fog1480Cmp)(uint32_t fn, uint32_t arg);

// 1.13c Fog+0x01480: binary-heap insert of the node at offset elem (stack arg). Growth happens when count + 1
// exceeds cap; the new capacity rounds up to a multiple of the granule. The sift walks parents while the
// compare of the parent (its vtable slot 0, called with base + elem) returns zero; each moved node takes its
// new slot index and the heap pointer.
void fog_1480_heap_insert(Fog1480Heap* h, uint32_t elem, Fog1480Granule granule, Fog1480Resize resize, Fog1480Cmp cmp)
{
    uint32_t cur = h->count;
    uint32_t nc = cur + 1;
    if (h->cap < nc) {
        uint32_t g = h->granule;
        if (g == 0)
            g = granule(h);
        uint32_t cap = nc;
        if (nc % g != 0)
            cap = (g - nc % g) + nc;
        resize(cap);
    }
    h->count = nc;
    uint32_t* slots = (uint32_t*)(uintptr_t)h->slots;
    while (cur != 0) {
        uint32_t parent = (cur - 1) >> 1;
        uint32_t parent_node = h->base + slots[parent];
        uint32_t fn = *(uint32_t*)(uintptr_t)*(uint32_t*)(uintptr_t)parent_node;
        if (cmp(fn, h->base + elem) != 0)
            break;
        slots[cur] = slots[parent];
        uint32_t* node = (uint32_t*)(uintptr_t)(h->base + slots[cur]);
        node[2] = cur;
        node[1] = (uint32_t)(uintptr_t)h;
        cur = parent;
    }
    slots[cur] = elem;
    uint32_t* node = (uint32_t*)(uintptr_t)(h->base + elem);
    node[2] = cur;
    node[1] = (uint32_t)(uintptr_t)h;
}
