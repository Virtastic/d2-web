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

// 1.13c D2Gfx+0x01340: unlink every pending slot. Retail reloads the buffer
// pointer and the pending offset on each pass because the linked write can
// change either; malformed state can loop, as it does in retail.
bool unlink_pending(const D2GfxGuestMemory& mem, uint32_t owner) {
    while (true) {
        uint32_t buffer, pending;
        if (!read32(mem, owner, buffer) || !read32(mem, owner + 8, pending)) return false;
        if (static_cast<int32_t>(pending) <= 0) return true;
        if (!unlink_slot(mem, buffer + pending)) return false;
    }
}

extern "C" bool d2gfx_unlink_slot(D2GfxGuestMemory mem, uint32_t slot);
extern "C" bool d2gfx_unlink_fixups_pending(D2GfxGuestMemory mem, uint32_t owner) { return unlink_pending(mem, owner); }
// 1.13c D2Gfx+0x010E0: unlink the pending slot(s), then the owner's first
// node.
extern "C" EMSCRIPTEN_KEEPALIVE bool d2gfx_unlink_fixups(D2GfxGuestMemory mem, uint32_t owner) {
    return d2gfx_unlink_fixups_pending(mem, owner) && d2gfx_unlink_slot(mem, owner + 4);
}

// 1.13c D2Gfx+0x01250: the same one-slot unlink, extracted by the compiler.
// Retail receives the slot address in EAX from the four C++ EH unwind
// funclets at 0x0CB80/0xCBA5/0xCBC0/0xCBE0; no arguments, no return value.
extern "C" EMSCRIPTEN_KEEPALIVE bool d2gfx_unlink_slot(D2GfxGuestMemory mem, uint32_t slot) {
    return unlink_slot(mem, slot);
}

// SEH unwind funclets: each loads its arg (the unwound object) and tail-calls the unlink above.
// 1.13c D2Gfx+0xCBC0: d2gfx_unlink_slot(obj + 4)
extern "C" bool d2gfx_unwind_cbc0(D2GfxGuestMemory mem, uint32_t obj) { return unlink_slot(mem, obj + 4); }
// 1.13c D2Gfx+0xCBE0: d2gfx_unlink_slot(obj + 4)
extern "C" bool d2gfx_unwind_cbe0(D2GfxGuestMemory mem, uint32_t obj) { return unlink_slot(mem, obj + 4); }
// 1.13c D2Gfx+0xCBEB: d2gfx_unlink_fixups(obj)
extern "C" bool d2gfx_unwind_cbeb(D2GfxGuestMemory mem, uint32_t obj) { return d2gfx_unlink_fixups(mem, obj); }
// 1.13c D2Gfx+0xCC00 (SEH funclet): d2gfx_unlink_fixups(*(frame+4)); the owner pointer is read from the unwound frame.
extern "C" bool d2gfx_unwind_cc00(D2GfxGuestMemory mem, uint32_t owner) { return d2gfx_unlink_fixups(mem, owner); }
