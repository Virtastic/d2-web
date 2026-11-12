// Prior art used: D2MOO 1.10f D2CMP/src/{LRUCache,Codec,FastCmp}.cpp
// and CelDataHash.cpp are empty module maps, not implementations. Rules below
// derive from cached 1.13c decompiles in build-wasm/ghidra-d2cmp/decomp/.
// Stage A only: native checks are not retail-vector parity evidence.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "d2cmp_cache_queues.h"

static D2CMPQueueNode* pop_unlocked(D2CMPQueuePair& queue) {
    D2CMPQueueNode* node = queue.head;
    if (!node || (node->flags & 2u)) return nullptr;
    queue.head = node->next;
    if (queue.head) queue.head->previous = nullptr;
    else queue.tail = nullptr;
    node->next = nullptr;
    node->previous = nullptr;
    return node;
}

// ECX was unused, EDX the cache. Only the pending head is considered; a locked
// head blocks the queue even if a later node is unlocked.
// 1.13c D2CMP+0x0BF00
extern "C" D2CMPQueueNode* d2cmp_bf00(D2CMPCacheQueues* cache) {
    return pop_unlocked(cache->pending);
}

// EAX selects the alternate queue when nonzero, EDX supplies the cache.
// 1.13c D2CMP+0x0bf40
extern "C" D2CMPQueueNode* d2cmp_bf40(D2CMPCacheQueues* cache, uint32_t alternate) {
    return pop_unlocked(alternate ? cache->alternate : cache->normal);
}

// ECX cache, EAX node. Retail does not clear node links on the empty path,
// nor node->next on the nonempty path: callers provide a detached node.
// 1.13c D2CMP+0x0BFB0
extern "C" void d2cmp_bfb0(D2CMPCacheQueues* cache, D2CMPQueueNode* node) {
    auto& queue = cache->pending;
    if (!queue.tail) {
        queue.head = node;
        queue.tail = node;
        return;
    }
    queue.tail->next = node;
    node->previous = queue.tail;
    queue.tail = node;
}

static int32_t scan_run(const uint8_t* cursor, const uint8_t* end,
                        uint8_t value, int32_t maximum, bool equal,
                        uint32_t* reachedDuringCount, uint32_t* reachedAfterSkip) {
    uint32_t count = 0;
    while (cursor < end && ((*cursor == value) == equal) &&
           static_cast<int32_t>(count) < maximum) {
        ++count;
        ++cursor;
    }
    if (cursor == end) {
        *reachedDuringCount = 1;
        *reachedAfterSkip = 0;
    } else {
        *reachedDuringCount = 0;
        while (cursor < end && *cursor == value) ++cursor;
        *reachedAfterSkip = static_cast<uint32_t>(cursor == end);
    }
    return static_cast<int32_t>(count);
}

// AL value, EDX end, stack cursor/maximum, EDI/EBX output words.
// 1.13c D2CMP+0x11350
extern "C" int32_t d2cmp_11350(const uint8_t* end, const uint8_t* cursor,
                               int32_t maximum, uint8_t value,
                               uint32_t* reachedDuringCount, uint32_t* reachedAfterSkip) {
    if (end <= cursor) std::_Exit(-1); // retail fatal line 0x6f
    return scan_run(cursor, end, value, maximum, false, reachedDuringCount, reachedAfterSkip);
}

// ECX cursor, EDX end, EAX maximum, stack/EDI output words.
// 1.13c D2CMP+0x11450
extern "C" int32_t d2cmp_11450(const uint8_t* cursor, const uint8_t* end,
                               int32_t maximum, uint32_t* reachedAfterSkip,
                               uint32_t* reachedDuringCount) {
    if (end <= cursor) std::_Exit(-1); // retail fatal line 0x19
    return scan_run(cursor, end, *cursor, maximum, true, reachedDuringCount, reachedAfterSkip);
}

// EAX source, EDX destination, stack row count / pitch. 0x80 ends a row,
// 0x81..0xff skip transparent pixels, 0x00..0x7f copy literal bytes.
// Retail's do/while row loop assumes positive rows and valid encoded input.
// 1.13c D2CMP+0x0FB50
extern "C" void d2cmp_fb50(const uint8_t* source, uint8_t* destination,
                           uint32_t rows, int32_t pitch) {
    uint8_t* nextRow = destination + pitch;
    do {
        for (;;) {
            const uint8_t command = *source++;
            if (command < 0x80u) {
                // Preserve retail's forward-copy ordering, including overlap.
                const uint32_t words = command >> 2;
                for (uint32_t i = 0; i < words; ++i) {
                    uint32_t word;
                    std::memcpy(&word, source + i * 4u, 4);
                    std::memcpy(destination + i * 4u, &word, 4);
                }
                for (uint32_t i = words * 4u; i < command; ++i) destination[i] = source[i];
                source += command;
                destination += command;
            } else if (command != 0x80u) {
                destination += command & 0x7fu;
            } else break;
        }
        --rows; // explicit retail unsigned wrap for the invalid zero-row input
        if (!rows) break;
        destination = nextRow;
        nextRow += pitch;
    } while (true);
}
