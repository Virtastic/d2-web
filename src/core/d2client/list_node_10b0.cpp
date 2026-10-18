#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x10B0 helper: unlink a node {u32 next; u32 prev-encoded} of the intrusive list in
// 0x1030/0x11F0. prev is either a pointer stored complemented (~v) or a roll-forward value decoded as
// node + (v - next->prev); both wrap in 32 bits.
static void unlink_10b0(uint32_t* node) {
    uint32_t next = node[0];
    if (!next) return;
    uint32_t prev = node[1];
    uintptr_t slot;
    if (static_cast<int32_t>(prev) < 0) slot = static_cast<uintptr_t>(~prev);
    else slot = reinterpret_cast<uintptr_t>(node) + (prev - *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(next) + 4));
    *reinterpret_cast<uint32_t*>(slot) = next;
    *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(next) + 4) = prev;
    node[0] = 0;
    node[1] = 0;
}

// 1.13c D2Client+0x10B0: ECX = this, EAX = container base; stack arg1 = node (0 -> EAX+1), arg2 = tag.
// Unlinks arg1-or-EAX+4 when non-empty, then (arg2 != 1) links it after ECX/EAX+4's next node or
// (arg2 == 1) before it, storing arg1 (raw, 0 included) as the neighbour's prev word.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_10b0(uint32_t* ecxThis, uint32_t* eaxBase, uint32_t* arg1, int32_t arg2) {
    uint32_t* node = arg1 ? arg1 : eaxBase + 1;
    unlink_10b0(node);
    uint32_t* self = ecxThis ? ecxThis : eaxBase + 1;
    if (arg2 != 1) {
        uint32_t n = self[0];
        node[0] = n;
        node[1] = *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(n) + 4);
        *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(n) + 4) = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(arg1));
        self[0] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(node));
        return;
    }
    node[0] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(self));
    node[1] = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(self) + 4);
    uint32_t prev = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(self) + 4);
    int32_t off = static_cast<int32_t>(eaxBase[0]);
    if (static_cast<int32_t>(prev) < 0) {
        *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(~prev)) = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(node));
        *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(self) + 4) = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(arg1));
        return;
    }
    if (off < 0) {
        uint32_t first = *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(self[0]) + 4);
        off = static_cast<int32_t>(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(self)) - first);
    }
    *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(prev + static_cast<uint32_t>(off))) = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(node));
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(self) + 4) = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(arg1));
}
