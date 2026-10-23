// 1.13c Fog+0x0D0A0 -- refcounted hash-bucket release: find node by key, drop one ref, unlink+free at <=0.
#include <cstdint>
#include <cstdlib>

struct FogHashNode { uint32_t key; int32_t refs; FogHashNode* next; };
struct FogHashTable { uint32_t nBuckets; FogHashNode** buckets; };  // retail: +0x10 count, +0x40 buckets

// Returns 1 if key found (ref dropped), 0 if absent.
int fog_hashRelease(FogHashTable* t, const uint32_t* key) {
    uint32_t b = *key % t->nBuckets;
    FogHashNode* prev = nullptr;
    FogHashNode* n = t->buckets[b];
    for (; n; prev = n, n = n->next)
        if (n->key == *key) break;
    if (!n) return 0;
    if (--n->refs < 1) {
        (prev ? prev->next : t->buckets[b]) = n->next;
        free(n);  // retail FUN_6ff6b0c0 (Fog free)
    }
    return 1;
}
