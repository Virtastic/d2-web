#pragma once
#include "chain_record.h"
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <limits>

namespace d2::quests {
// Tokens are serialized 32-bit addresses, never native quest/node identities.
// Memory::resolve(token) maps a token to backing bytes; allocate(size) must return
// a token resolving to an owned allocation, released by that memory owner.
template<class Memory>
struct SerializedChainStorage {
    Memory& memory;
    uint32_t read(uint32_t token, unsigned offset) const {
        uint32_t value;
        std::memcpy(&value, memory.resolve(token) + offset, sizeof value);
        return value;
    }
    void write(uint32_t token, unsigned offset, uint32_t value) const {
        std::memcpy(memory.resolve(token) + offset, &value, sizeof value);
    }
    uint32_t quest(uint32_t token) const { return read(token, 0); }
    uint32_t next(uint32_t token) const { return read(token, 4); }
    uint32_t allocate() { return memory.allocate(8); }
    void initialize(uint32_t token, uint32_t questToken, uint32_t nextToken) const {
        write(token, 0, questToken);
        write(token, 4, nextToken);
    }
};

// Legacy FN blobs use absolute linear-memory addresses as tokens. This adapter
// cannot represent allocations outside that address space: fail rather than
// silently aliasing a different allocation. Native game lists use native pointers.
struct LinearTokenMemory {
    unsigned char* resolve(uint32_t token) const {
        return reinterpret_cast<unsigned char*>(static_cast<uintptr_t>(token));
    }
    uint32_t allocate(unsigned size) const {
        void* block = std::calloc(1, size);
        const uintptr_t address = reinterpret_cast<uintptr_t>(block);
        if (!block || address > std::numeric_limits<uint32_t>::max()) {
            std::free(block);
            std::abort();
        }
        return static_cast<uint32_t>(address);
    }
};
}
