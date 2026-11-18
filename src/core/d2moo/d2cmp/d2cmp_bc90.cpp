// Prior art: none in D2MOO 1.10f (LRUCache only includes). Rules from cached 1.13c body.
// Retail's hidden register args (EAX size, EBX owner, ESI cache) are explicit; unmapped
// callees (8EF0, 8F70, 15C10) and Storm ordinals 10102/10103 are injected hooks.
#include <cstdint>
#include <cstring>
#include "d2cmp_tile_lru113c.h"

struct D2CmpBc90Obj {          // retail offsets in comments
    uint8_t* data;             // +54 (native pointer in place of the dword)
    D2CmpLruNode113c* node;    // +5c
    uint32_t size;             // +4c
    int32_t count;             // +50
    const char* name;          // +58
};
struct D2CmpBc90Ops {
    D2CmpLruNode113c* (*alloc)(D2CmpLru113c*, uint32_t size, void* owner); // 174B0
    int32_t (*readFile)(D2CmpBc90Obj&);   // Ordinal_10102, nonzero = ok
    void (*afterRead)(D2CmpBc90Obj&);     // 8F70, 8EF0, Ordinal_10103
    void (*fixup)(D2CmpBc90Obj&);         // 15C10: entry +0x10 += data base, stride 0x14
    void (*fail)(const char*);            // 28D0 / Ordinal_10029
};

// 1.13c D2CMP+0xBC90
extern "C" uint32_t d2cmp_bc90(D2CmpLru113c* cache, D2CmpBc90Obj& o, const D2CmpBc90Ops& ops) {
    if (o.node) { d2cmp_173a0(cache, o.node); return 1; }
    if (!o.count) return 0;
    o.node = ops.alloc(cache, o.size, &o);
    o.data = (uint8_t*)o.node->data;
    std::memset(o.data, 0, o.size);
    if (ops.readFile(o)) { ops.afterRead(o); ops.fixup(o); return 1; }
    ops.fail("Error opening file: %s");
    return 0;
}
