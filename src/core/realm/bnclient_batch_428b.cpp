// BNClient VC7.1 CRT lifecycle. Prior art: D2MCPClient's matching CRT lifecycle
// (mcp_crt_batch_b56.cpp); D2MOO has no BNClient implementation. Contracts checked
// against cached 1.13c bodies 428B/46E8/4739/49C9. No parity claim (Stage A).
#include <cstddef>
#include <cstdint>

// The handle array is the caller's live CRT table, not a copied state snapshot.
struct BnIoRecord {
    int32_t handle;
    uint8_t flags, token, padding[2];
    int32_t initialized;
    uint8_t critical_section[24];
};
static_assert(sizeof(BnIoRecord) == 0x24);
static_assert(offsetof(BnIoRecord, initialized) == 8);
static_assert(offsetof(BnIoRecord, critical_section) == 12);

// 1.13c BNClient+0x0428B
// Destroy initialized locks, release all 64 blocks, clear each native owner slot.
// Retail deliberately leaves its allocated-handle count unchanged.
extern "C" void bnclient_ioterm(BnIoRecord** blocks,
                                void (*delete_lock)(void*), void (*free_block)(void*)) {
    for (uint32_t slot = 0; slot != 64; ++slot) {
        if (blocks[slot] == nullptr) continue;
        for (uint32_t entry = 0; entry != 32; ++entry) {
            if (blocks[slot][entry].initialized != 0)
                delete_lock(blocks[slot][entry].critical_section);
        }
        free_block(blocks[slot]);
        blocks[slot] = nullptr;
    }
}

// Retail's five-dword small-block region header contains two pointers. Native
// pointers remain native here; bitmap/count words retain retail widths.
struct BnHeapRegion {
    uint32_t available_low, available_high, committed;
    void* reservation;
    void* metadata;
};
struct BnHeapOwner {
    void* heap;                 // +0x20BE0
    uint32_t mode;              // +0x20BE4
    uint32_t deferred;          // +0x20BC4
    int32_t region_count;       // +0x20BC8
    BnHeapRegion* regions;      // +0x20BCC
    uint32_t threshold;         // +0x20BD0
    BnHeapRegion* next_region;  // +0x20BD4
    uint32_t capacity;          // +0x20BD8
};
struct BnHeapApi {
    void* (*create)(uint32_t, uint32_t, uint32_t);
    int32_t (*destroy)(void*);
    void* (*allocate)(void*, uint32_t, uint32_t);
    int32_t (*free)(void*, uint32_t, void*);
    int32_t (*virtual_free)(void*, uint32_t, uint32_t);
    int32_t (*select_heap)();
};

// 1.13c BNClient+0x049C9
// Allocate capacity for 16 native region records, publishing state only on success.
extern "C" int32_t bnclient_sbh_heap_init(BnHeapOwner* owner, const BnHeapApi* api,
                                         uint32_t threshold) {
    owner->regions = static_cast<BnHeapRegion*>(api->allocate(
        owner->heap, 0, static_cast<uint32_t>(16 * sizeof(BnHeapRegion))));
    if (owner->regions == nullptr) return 0;
    owner->deferred = 0;
    owner->region_count = 0;
    owner->next_region = owner->regions;
    owner->threshold = threshold;
    owner->capacity = 16;
    return 1;
}

// 1.13c BNClient+0x046E8
extern "C" int32_t bnclient_heap_init(BnHeapOwner* owner, const BnHeapApi* api,
                                     int32_t multithreaded) {
    owner->heap = api->create(multithreaded == 0, 0x1000, 0);
    if (owner->heap == nullptr) return 0;
    owner->mode = static_cast<uint32_t>(api->select_heap());
    if (owner->mode == 3 && bnclient_sbh_heap_init(owner, api, 0x3f8) == 0) {
        api->destroy(owner->heap);
        return 0;
    }
    return 1;
}

// 1.13c BNClient+0x04739
// Decommit then release each 1 MiB region, free its metadata and table, destroy
// the heap. Retail does not clear dangling globals after teardown.
extern "C" void bnclient_heap_term(BnHeapOwner* owner, const BnHeapApi* api) {
    if (owner->mode == 3) {
        for (int32_t i = 0; i < owner->region_count; ++i) {
            api->virtual_free(owner->regions[i].reservation, 0x100000, 0x4000);
            api->virtual_free(owner->regions[i].reservation, 0, 0x8000);
            api->free(owner->heap, 0, owner->regions[i].metadata);
        }
        api->free(owner->heap, 0, owner->regions);
    }
    api->destroy(owner->heap);
}
