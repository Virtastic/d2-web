// Prior art: D2MOO D2CMP/Tilecmp.cpp and FindTiles.cpp supply the subsystem map
// (their bodies are empty); D2Common's D2TileLibraryEntryStrc supplies the three
// scalar tile keys. Rules below come from the cached 1.13c bodies, checked against
// the DLL instructions. This index supports tile lookup, not a serialized layout.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>

struct D2CMPTileIndexLink {
    const void* tile;
    D2CMPTileIndexLink* next;
};
struct D2CMPTileIndexEntry {
    uint32_t key18, key1c, key14;
    D2CMPTileIndexLink* tiles;
    D2CMPTileIndexEntry* next;
};
struct D2CMPTileIndex { D2CMPTileIndexEntry* buckets[128]; };
// Fog #10042/#10043 allocation seams: size is the native object size;
// line numbers preserve the retail call identity. All links are native pointers.
struct D2CMPTileIndexMemory {
    void* (*allocate)(size_t bytes, uint32_t line);
    void (*release)(void*, uint32_t line);
};
static uint32_t tileKey(const void* p, size_t offset) {
    uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + offset, 4); return v;
}
[[noreturn]] static void tileIndexFatal(uint32_t) { std::_Exit(-1); }

// Stack bytes a,b,c; the x86 AL arithmetic wraps before zero extension. The
// decompile loses EAX's return and calls this void; the assembly returns the hash.
// 1.13c D2CMP+0x12E10
extern "C" uint32_t d2cmp_12e10(uint8_t a, uint8_t b, uint8_t c) {
    return static_cast<uint8_t>(uint32_t(a) * 2u - uint32_t(c) + uint32_t(b)) & 127u;
}
// EBX=&bucket, EDI=tile; no stack arguments. Publish entry before allocating
// its initial link, as retail does (including the fatal second allocation case).
// 1.13c D2CMP+0x12C30
extern "C" void d2cmp_12c30(D2CMPTileIndexEntry** bucket, const void* tile,
                            const D2CMPTileIndexMemory* memory) {
    auto* entry = static_cast<D2CMPTileIndexEntry*>(memory->allocate(sizeof(D2CMPTileIndexEntry), 0x82));
    if (!entry) tileIndexFatal(0x83);
    *entry = {tileKey(tile, 0x18), tileKey(tile, 0x1c), tileKey(tile, 0x14), nullptr, *bucket};
    *bucket = entry;
    auto* link = static_cast<D2CMPTileIndexLink*>(memory->allocate(sizeof(D2CMPTileIndexLink), 0x8e));
    if (!link) tileIndexFatal(0x8f);
    *link = {tile, nullptr};
    entry->tiles = link;
}
// ESI=entry, EDI=tile. Duplicate identity is pointer equality, not key equality.
// 1.13c D2CMP+0x12CD0
extern "C" void d2cmp_12cd0(D2CMPTileIndexEntry* entry, const void* tile,
                            const D2CMPTileIndexMemory* memory) {
    for (auto* p = entry->tiles; p; p = p->next) if (p->tile == tile) return;
    auto* link = static_cast<D2CMPTileIndexLink*>(memory->allocate(sizeof(D2CMPTileIndexLink), 0x6e));
    if (!link) tileIndexFatal(0x6f);
    *link = {tile, entry->tiles};
    entry->tiles = link;
}
// ECX=index, EAX=tile; match complete dwords after hashing their low bytes.
// 1.13c D2CMP+0x12F20
extern "C" void d2cmp_12f20(D2CMPTileIndex* index, const void* tile,
                            const D2CMPTileIndexMemory* memory) {
    if (!index) tileIndexFatal(0xa5);
    if (!tile) tileIndexFatal(0xa6);
    const uint32_t a = tileKey(tile, 0x1c), b = tileKey(tile, 0x18), c = tileKey(tile, 0x14);
    auto** bucket = &index->buckets[d2cmp_12e10(uint8_t(a), uint8_t(b), uint8_t(c))];
    for (auto* e = *bucket; e; e = e->next) {
        if (e->key1c == a && e->key18 == b && e->key14 == c) {
            d2cmp_12cd0(e, tile, memory); return;
        }
    }
    d2cmp_12c30(bucket, tile, memory);
}
// EAX=index, EBX=key1c; stack output,capacity,key14,key18 (callee pops 16).
// Checks precede the zero-capacity return. Output pointers retain native width.
// 1.13c D2CMP+0x12E60
extern "C" uint32_t d2cmp_12e60(const D2CMPTileIndex* index, uint32_t key1c,
                               const void** output, uint32_t capacity,
                               uint32_t key14, uint32_t key18) {
    if (!index) tileIndexFatal(0xcf);
    if (!output) tileIndexFatal(0xd0);
    if (!capacity) return 0;
    auto* e = index->buckets[d2cmp_12e10(uint8_t(key1c), uint8_t(key18), uint8_t(key14))];
    for (; e; e = e->next) {
        if (e->key1c != key1c || e->key18 != key18 || e->key14 != key14) continue;
        uint32_t n = 0;
        for (auto* p = e->tiles; p && n < capacity; p = p->next) output[n++] = p->tile;
        return n;
    }
    return 0;
}
// Stack index (callee pops four); release links, entries, then owning index.
// 1.13c D2CMP+0x12D30
extern "C" void d2cmp_12d30(D2CMPTileIndex* index, const D2CMPTileIndexMemory* memory) {
    if (!index) return;
    for (auto& bucket : index->buckets) {
        auto* e = bucket;
        while (e) {
            auto* next = e->next;
            auto* p = e->tiles;
            while (p) { auto* pn = p->next; memory->release(p, 0x4c); p = pn; }
            e->tiles = nullptr;
            memory->release(e, 0x50);
            e = next;
        }
        bucket = nullptr;
    }
    memory->release(index, 0x57);
}
