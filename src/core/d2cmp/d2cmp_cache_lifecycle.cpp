// Prior art used: D2MOO 1.10f D2CMP LRUCache.cpp and CelDataHash.cpp
// contain only module includes. Cached 1.13c bodies supply the rules below.
// Stage A: native pointer owners replace retail pointers, never packed dwords.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <new>

struct D2CMPCachePayload {
    void* allocation54; // retail payload +54
    void* allocation5c; // retail payload +5c
};
using D2CMPCachePayloadRelease = void (*)(D2CMPCachePayload&);

struct D2CMPLruNode {
    void* allocation;
    uint32_t bytes;
    D2CMPCachePayload* payload;
    D2CMPCachePayloadRelease* releasePayloadSlot;
    D2CMPLruNode* next;
    D2CMPLruNode* previous;
};
struct D2CMPLruCache {
    uint32_t identity;                  // retail +00
    uint32_t budget;                    // +04
    uint32_t used;                      // +08
    uint32_t count;                     // +0c
    D2CMPLruNode* nodes;                // +10, native allocation
    uint32_t capacity;                  // +14
    D2CMPLruNode* head;                 // +18
    D2CMPLruNode* tail;                 // +1c
    D2CMPLruNode* freeHead;             // +20
    D2CMPLruNode* freeTail;             // +24
    void (*destroyPayload)(D2CMPLruNode&); // +28; register argument made explicit
    uint32_t hits;                      // +2c
    uint32_t misses;                    // +30
};
static_assert(offsetof(D2CMPLruCache, count) == 12);
static_assert(offsetof(D2CMPLruNode, bytes) == sizeof(void*));
static_assert(sizeof(D2CMPLruNode::allocation) == sizeof(void*));

// Allocation/free are Fog #10045/#10046 boundaries. The cache owns its nodes;
// the host allocator must return suitably aligned memory, as Fog does.
struct D2CMPCacheMemory {
    void* (*allocate)(uint32_t identity, size_t bytes, const char* file, uint32_t line);
    void (*release)(uint32_t identity, void* allocation, const char* file, uint32_t line);
};
static constexpr const char* lruFile = "..\\Source\\D2CMP\\SRC\\LRUCache.cpp";

// ESI cache and EAX capacity were hidden register arguments in the decompile.
// 1.13c D2CMP+0x17270
extern "C" void d2cmp_17270(D2CMPLruCache& c, uint32_t identity, uint32_t budget,
    uint32_t capacity, void (*destroy)(D2CMPLruNode&), const D2CMPCacheMemory& memory) {
    c.identity = identity;
    c.budget = budget;
    c.destroyPayload = destroy;
    c.used = 0;
    c.capacity = capacity;
    c.nodes = static_cast<D2CMPLruNode*>(memory.allocate(c.identity, size_t(capacity) * sizeof(D2CMPLruNode), lruFile, 48));
    for (uint32_t i = 0; i < capacity; ++i) {
        new (c.nodes + i) D2CMPLruNode{};
        c.nodes[i].next = i + 1 < capacity ? c.nodes + i + 1 : nullptr;
        c.nodes[i].previous = i ? c.nodes + i - 1 : nullptr;
    }
    c.count = 0;
    c.head = c.tail = nullptr;
    c.hits = c.misses = 0;
    c.freeHead = c.nodes;
    // Retail computes base-20 for capacity zero. Preserve the address arithmetic
    // without forming an out-of-bounds C++ array pointer.
    c.freeTail = reinterpret_cast<D2CMPLruNode*>(reinterpret_cast<uintptr_t>(c.nodes)
        + uintptr_t(capacity) * sizeof(D2CMPLruNode) - sizeof(D2CMPLruNode));
}

// ESI cache, EDI size hidden register args. Takes a free node onto the active
// tail and charges size; null when full or over budget; fatal if no free node.
// 1.13c D2CMP+0x171E0
extern "C" D2CMPLruNode* d2cmp_171e0(D2CMPLruCache& c, uint32_t size) {
    if (c.count == c.capacity || c.budget < c.used + size) return nullptr;
    D2CMPLruNode* n = c.freeHead;
    if (n) {
        c.freeHead = n->next;
        if (c.freeHead) c.freeHead->previous = nullptr;
        else c.freeTail = nullptr;
        n->next = n->previous = nullptr;
    }
    if (!c.tail) c.head = n;
    else if (n) { c.tail->next = n; n->previous = c.tail; }
    else c.tail->next = n;
    c.tail = n;
    if (!n) std::_Exit(-1);
    ++c.count;
    c.used += size;
    return n;
}

// EAX size, EBX owner, ESI cache hidden register args (EDI = size for 171E0).
// Evicts from the head until a node fits, then allocates the node's block.
// 1.13c D2CMP+0x174b0
extern "C" D2CMPLruNode* d2cmp_174b0(D2CMPLruCache& c, uint32_t size, D2CMPCachePayload* owner,
    const D2CMPCacheMemory& memory) {
    if (c.budget <= size) std::_Exit(-1);
    ++c.misses;
    ++c.hits;
    D2CMPLruNode* n;
    while (!(n = d2cmp_171e0(c, size))) {
        if (c.count) {
            if ((n = c.head)) {
                c.head = n->next;
                if (c.head) c.head->previous = nullptr;
                else c.tail = nullptr;
                n->next = n->previous = nullptr;
                c.used -= n->bytes;
                if (n->payload) c.destroyPayload(*n);
                if (n->allocation) {
                    memory.release(c.identity, n->allocation, lruFile, 200);
                    n->allocation = nullptr;
                    n->bytes = 0;
                    n->payload = nullptr;
                }
                if (!c.freeTail) c.freeHead = n;
                else {
                    c.freeTail->next = n;
                    n->previous = c.freeTail;
                }
                c.freeTail = n;
            }
            --c.count;
        }
    }
    n->allocation = memory.allocate(c.identity, size, lruFile, 0x84);
    if (!n->allocation) std::_Exit(-1);
    n->bytes = size;
    n->payload = owner;
    return n;
}

// EAX node (null = head), EDI cache hidden register args. Unlinks the node
// from the active list (17320 semantics), releases it, appends to the free tail.
// 1.13c D2CMP+0x17400
extern "C" void d2cmp_17400(D2CMPLruCache& c, D2CMPLruNode* n, const D2CMPCacheMemory& memory) {
    if (!c.count) return;
    if (!n) {
        n = c.head;
        if (!n) { --c.count; return; }
        c.head = n->next;
        if (c.head) c.head->previous = nullptr; else c.tail = nullptr;
        n->next = n->previous = nullptr;
    } else if (c.head) {
        if (n == c.head) {
            c.head = n->next;
            if (c.head) c.head->previous = nullptr; else c.tail = nullptr;
        } else if (n == c.tail) {
            c.tail = n->previous;
            c.tail->next = nullptr;
        } else {
            n->previous->next = n->next;
            n->next->previous = n->previous;
        }
        n->next = n->previous = nullptr;
    }
    c.used -= n->bytes;
    if (n->payload) c.destroyPayload(*n);
    if (n->allocation) {
        memory.release(c.identity, n->allocation, lruFile, 200);
        n->allocation = nullptr;
        n->bytes = 0;
        n->payload = nullptr;
    }
    if (!c.freeTail) { c.freeHead = c.freeTail = n; }
    else {
        c.freeTail->next = n;
        n->previous = c.freeTail;
        c.freeTail = n;
    }
    --c.count;
}

// EDI cache was a hidden register argument. Count is decremented even when
// no head exists; freeing the node table does not clear the other queue fields.
// 1.13c D2CMP+0x175c0
extern "C" void d2cmp_175c0(D2CMPLruCache& c, const D2CMPCacheMemory& memory) {
    while (c.count) {
        if (D2CMPLruNode* n = c.head) {
            c.head = n->next;
            if (c.head) c.head->previous = nullptr;
            else c.tail = nullptr;
            n->next = n->previous = nullptr;
            c.used -= n->bytes;
            if (n->payload) c.destroyPayload(*n);
            if (n->allocation) {
                memory.release(c.identity, n->allocation, lruFile, 200);
                n->allocation = nullptr;
                n->bytes = 0;
                n->payload = nullptr;
            }
            if (!c.freeTail) c.freeHead = n;
            else {
                c.freeTail->next = n;
                n->previous = c.freeTail;
            }
            c.freeTail = n;
        }
        --c.count;
    }
    if (c.nodes) {
        memory.release(c.identity, c.nodes, lruFile, 99);
        c.nodes = nullptr;
    }
}

struct D2CMPCelCacheEntry {
    uint32_t key[3];                  // retail cel +00,+04,+08
    uint32_t flags;                   // cel +20
    void* resource;                  // cel +b0
};
struct D2CMPCelCacheLink {
    D2CMPCelCacheEntry* cel;          // retail node +00
    D2CMPCelCacheLink* next;          // retail node +14
};
struct D2CMPCacheLifecycle {
    D2CMPLruCache cache;             // retail 6fe872b0..6fe872e0
    D2CMPCachePayloadRelease releasePayload; // retail global 6fe87f10
    uint32_t enabled;                // 6fe87f08
    uint32_t budget;                 // 6fe87f0c
    D2CMPCelCacheLink* celHead;       // 6fe7725c
    uint32_t tableEnabled;           // 6fe32cc8
    const uint8_t* activeTable;      // 6fe32ccc
    const uint8_t* fallbackTable;    // 6fe32cd0
};
struct D2CMPCelResourceOps {
    int32_t (*busy)(void* resource); // D2CMP #10092
    void (*release)(void* resource); // D2CMP #10097
};

// ECX is the LRU node; its +08 points at the cel payload. The global
// callback is read again after validation in retail (IsBadCodePtr); native
// function pointers are already callable. Clear fields after callback return.
// 1.13c D2CMP+0x0BB60
extern "C" void d2cmp_bb60(D2CMPLruNode& node) {
    auto* payload = node.payload;
    if (*node.releasePayloadSlot) (*node.releasePayloadSlot)(*payload);
    payload->allocation54 = nullptr;
    payload->allocation5c = nullptr;
}

// Retail fixes the destroy routine to BB60, rather than accepting a caller
// supplied destructor. All nodes read the same mutable global callback slot.
// 1.13c D2CMP+0x0BBB0
extern "C" void d2cmp_bbb0(D2CMPCacheLifecycle& s, uint32_t identity, uint32_t budget,
    const D2CMPCacheMemory& memory) {
    s.cache = {};
    s.budget = budget;
    d2cmp_17270(s.cache, identity, budget, 0x1000, d2cmp_bb60, memory);
    for (uint32_t i = 0; i < s.cache.capacity; ++i)
        s.cache.nodes[i].releasePayloadSlot = &s.releasePayload;
    s.enabled = 1;
}

// 1.13c D2CMP+0x0BA60
extern "C" void d2cmp_ba60(D2CMPCacheLifecycle& s) {
    s.cache.misses = 0;
    s.cache.hits = 0;
}

// Store ordering matters when the caller aliases output words.
// 1.13c D2CMP+0x0BA70
extern "C" void d2cmp_ba70(const D2CMPCacheLifecycle& s, uint32_t* budget,
    uint32_t* hits, uint32_t* misses, uint32_t* percent) {
    *budget = s.budget;
    *misses = s.cache.misses;
    *hits = s.cache.hits;
    *percent = s.cache.budget ? (s.cache.used * uint32_t(100)) / s.cache.budget : UINT32_MAX;
}

// 1.13c D2CMP+0x0BE10
extern "C" uint32_t d2cmp_be10(D2CMPCacheLifecycle& s, uint32_t a, uint32_t b,
    uint32_t c, const D2CMPCelResourceOps& ops) {
    for (auto* n = s.celHead; n; n = n->next) {
        auto* cel = n->cel;
        if (cel && !(cel->flags & 1) && cel->key[2] == c && cel->key[1] == b && cel->key[0] == a) {
            if (!cel->resource) std::_Exit(-1); // retail fatal 0xbc5
            if (!ops.busy(cel->resource)) return 1;
        }
    }
    return 0;
}

// 1.13c D2CMP+0x0BDB0
extern "C" void d2cmp_bdb0(D2CMPCacheLifecycle& s, const D2CMPCelResourceOps& ops) {
    for (auto* n = s.celHead; n; n = n->next) {
        auto* cel = n->cel;
        if (cel) {
            // Retail holds +b0 in EDI across the busy callback.
            void* resource = cel->resource;
            if (resource && !ops.busy(resource)) {
                ops.release(resource);
                cel->resource = nullptr;
            }
        }
    }
}

// 1.13c D2CMP+0x0BC60
extern "C" void d2cmp_bc60(D2CMPLruCache& cache, const D2CMPCacheMemory& memory) {
    d2cmp_175c0(cache, memory);
}

// HOLD BD70: requires native 9880 registry shutdown, including tile cleanup
// 16600 and hash teardown 12D30. No implementation or source marker is claimed
// while those essential callees lack a compatible native owner binding.

// 1.13c D2CMP+0x0C9D0
extern "C" const uint8_t* d2cmp_c9d0(const D2CMPCacheLifecycle& s, uint32_t alternate) {
    return alternate && s.tableEnabled ? s.activeTable : s.fallbackTable;
}

struct D2CMPCelFrameOwner {
    int32_t xOffset;                  // retail frame +0c
    int32_t yOffset;                  // +10
    void* resource;                   // +18
};
struct D2CMPCelFileOwner {
    uint32_t version;                 // retail +00
    uint32_t flags;                   // +04
    uint32_t codec;                   // +08
    uint32_t directions;              // +10
    uint32_t framesPerDirection;      // +14
    D2CMPCelFrameOwner** frames;       // native array, retail inline from +18
};
struct D2CMPCelDrawOwner {
    D2CMPCelFileOwner* file;           // retail context +34
    D2CMPCelFrameOwner* frame;         // +3c
};
struct D2CMPCelCloseOps {
    // Retail rereads this global callback after every call: callbacks may
    // replace it. Passing a pointer to the native callback slot preserves that.
    void (**releaseFrame)(void* resource);
    void (*releaseFile)(void* allocation, uint32_t line, uint32_t flags);
};

// 1.13c D2CMP+0x11520
extern "C" uint32_t d2cmp_11520(D2CMPCelFileOwner* file, const D2CMPCelCloseOps& ops) {
    if (!file) return 0;
    if (file->version != 6) std::_Exit(-1); // retail fatal 0x5f1
    auto** frames = file->frames; // EDI walks the original inline frame slots.
    void* allocation = frames[0]->resource; // saved across all callbacks
    if (!allocation) std::_Exit(-1); // retail fatal 0x5f5
    for (uint32_t i = 0; i < file->directions * file->framesPerDirection; ++i) {
        auto* frame = frames[i];
        if (frame && frame->resource) {
            if (*ops.releaseFrame) (*ops.releaseFrame)(frame->resource);
            // Callback may replace the frame pointer in the file.
            frames[i]->resource = nullptr;
        }
    }
    // Retail saves the initial frame resource over the incoming stack argument
    // (155f), then restores it as Fog #10043's owner argument (15cb).
    ops.releaseFile(allocation, 0x61d, 0);
    return 1;
}

using D2CMPCelStripDraw = void (*)(int32_t x, uint32_t a, uint32_t b,
    uint32_t c, int32_t y, int32_t height, uint32_t d, uint32_t e, uint32_t f);
static int32_t add32(int32_t a, int32_t b) { return static_cast<int32_t>(uint32_t(a) + uint32_t(b)); }
static int32_t sub32(int32_t a, int32_t b) { return static_cast<int32_t>(uint32_t(a) - uint32_t(b)); }

// Vertical strip clipping. Width is only a required nonzero input; retail does
// not clip x here. Arithmetic retains x86 wrap before the signed comparisons.
// 1.13c D2CMP+0x11780
extern "C" void d2cmp_11780(const D2CMPCelDrawOwner& context,
    int32_t x, int32_t y, uint32_t a, uint32_t b, int32_t width,
    int32_t height, uint32_t c, uint32_t d, uint32_t e,
    int32_t bottom, int32_t stripHeight, uint32_t f,
    const D2CMPCelStripDraw* codecs) {
    auto* file = context.file;
    if (file) {
        if (file->version != 6) std::_Exit(-1); // 0x4c6
        if (file->flags & 4) std::_Exit(-1);   // 0x4c7
    }
    if (!width) std::_Exit(-1);              // 0x4ca
    auto* frame = context.frame;
    if (!frame) std::_Exit(-1);              // 0x4cd
    const int32_t frameY = add32(y, frame->yOffset);
    int32_t end = sub32(frameY, bottom);
    int32_t start = sub32(end, stripHeight);
    if (end >= 0 && start < height) {
        if (start < 0) start = 0;
        if (height <= end) end = sub32(height, 1);
        const int32_t rows = sub32(end, start);
        if (rows) {
            auto draw = codecs[file->codec];
            if (!draw) std::_Exit(-1);       // 0x4e9
            draw(add32(x, frame->xOffset), a, b, c, sub32(frameY, end), rows, d, e, f);
        }
    }
}
