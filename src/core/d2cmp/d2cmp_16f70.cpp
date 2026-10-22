// D2CMP GfxHash node insert. Native pointers replace retail dwords; hash (16E80) and alloc injected.
#include <cstdint>
#include <cstddef>

struct GhNode {
    uint32_t bucket, id, k[5], hash, _20;   // +0, +4, +8..+18, +1C, +20 (retail +0x24 slot unused here)
    GhNode *prev, *next;                    // retail +28, +2C
};
struct GhKey { uint32_t _0, flags, kind, idA, _10, idB, k[5]; };  // retail +4,+8,+C,+14,+18..+28

// 1.13c D2CMP+0x16F70
// Retail: EBX = table (bucket heads at +0x8000), EDI = key; hash/bucket from sub_16E80 on the hash slot.
GhNode* d2cmp_16f70(GhNode** buckets, const GhKey& key, GhNode* (*alloc)(), uint32_t (*hashKey)(uint32_t& outHash)) {
    GhNode* n = alloc();                    // Ordinal_10045(GfxHash.cpp, 0xef)
    n->_20 = 0;
    uint32_t b = hashKey(n->hash);
    for (int i = 0; i < 5; i++) n->k[i] = key.k[i];
    if (key.kind == 6 && (key.flags & 2)) n->id = key.idB;
    else if (key.kind == 4 || key.kind == 3) n->id = key.idA;
    n->prev = nullptr;
    n->next = buckets[b];
    if (n->next) n->next->prev = n;
    n->bucket = b;
    buckets[b] = n;
    return n;
}
