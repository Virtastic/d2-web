// D2Gfx CRT relocation cleanup. Guest addresses remain 32-bit even on wasm64.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GfxGuestMemory {
    uint8_t* bytes;
    uint32_t base;
    uint32_t size;
};

#pragma pack(push, 1)
struct D2GfxFixupOwner {
    uint32_t buffer;
    uint32_t first_node;
    int32_t pending_offset;
};
struct D2GfxFixupSlot {
    uint32_t node;
    uint32_t back_reference;
};
#pragma pack(pop)
static_assert(offsetof(D2GfxFixupOwner, buffer) == 0);
static_assert(offsetof(D2GfxFixupOwner, first_node) == 4);
static_assert(offsetof(D2GfxFixupOwner, pending_offset) == 8);
static_assert(offsetof(D2GfxFixupSlot, node) == 0);
static_assert(offsetof(D2GfxFixupSlot, back_reference) == 4);

static bool read32(const D2GfxGuestMemory& mem, uint32_t addr, uint32_t& value) {
    const uint32_t offset = addr - mem.base;
    if (addr < mem.base || offset > mem.size || mem.size - offset < 4) return false;
    std::memcpy(&value, mem.bytes + offset, 4);
    return true;
}
static bool write32(const D2GfxGuestMemory& mem, uint32_t addr, uint32_t value) {
    const uint32_t offset = addr - mem.base;
    if (addr < mem.base || offset > mem.size || mem.size - offset < 4) return false;
    std::memcpy(mem.bytes + offset, &value, 4);
    return true;
}

// Undo one linked relocation. A negative back-reference encodes the complement
// of an absolute address; a nonnegative one is relative to the node's +4 field.
static bool unlink_slot(const D2GfxGuestMemory& mem, uint32_t slot) {
    uint32_t node, back, node_link;
    if (!read32(mem, slot, node) || !read32(mem, slot + 4, back)) return false;
    if (!node) return true;
    if (!read32(mem, node + 4, node_link)) return false;
    const uint32_t target = static_cast<int32_t>(back) < 0
        ? ~back : (slot + back - node_link);
    return write32(mem, target, node) && write32(mem, node + 4, back) &&
           write32(mem, slot, 0) && write32(mem, slot + 4, 0);
}

// 1.13c D2Gfx+0x010E0: unlink the pending slot(s), then the owner's first
// node. The loop deliberately rereads pending_offset after each unlink: retail
// expects the linked write to update it, and can loop on malformed state.
extern "C" EMSCRIPTEN_KEEPALIVE bool d2gfx_unlink_fixups(D2GfxGuestMemory mem, uint32_t owner) {
    uint32_t pending, buffer;
    if (!read32(mem, owner, buffer)) return false;
    while (true) {
        if (!read32(mem, owner + 8, pending)) return false;
        if (static_cast<int32_t>(pending) <= 0) break;
        if (!unlink_slot(mem, buffer + pending)) return false;
    }
    return unlink_slot(mem, owner + 4);
}
