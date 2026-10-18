#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x2400 (166 B): doubly-linked-list splice. Same two-word node encoding as
// 0x10B0/0x1030 -- [0] successor (0 = unlinked), [1] predecessor, negative = ~target, otherwise
// node + (enc - successor[1]). Differences from 0x10B0: the node and anchor are arguments offset
// into base[0] when non-zero (0 selects the embedded sentinel at base+4), the raw argument word is
// stored in the back-link, and base[0] is reloaded at each of its three uses. The unlink reloads
// the node words after the first store because that store can alias the node. ABI (retail):
// EAX = base, EBX = node argument, stack = mode, anchor argument.
static void unlink_2400(uint32_t node) {
    uint32_t next = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(node));
    if (next == 0) return;
    uint32_t enc = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(node + 4));
    uint32_t target;
    if (static_cast<int32_t>(enc) < 0) {
        target = ~enc;
    } else {
        target = node + (enc - *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(next + 4)));
    }
    *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(target)) = next;
    next = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(node));
    enc = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(node + 4));
    *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(next + 4)) = enc;
    *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(node)) = 0;
    *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(node + 4)) = 0;
}

// 1.13c D2Client+0x2400
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_2400(uint32_t base, uint32_t node_arg, uint32_t mode, uint32_t anchor_arg) {
    uint32_t node = node_arg
        ? *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(base)) + node_arg
        : base + 4;
    unlink_2400(node);

    uint32_t anchor = anchor_arg
        ? *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(base)) + anchor_arg
        : base + 4;

    if (mode != 1) {
        uint32_t first = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(anchor));
        *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(node)) = first;
        *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(node + 4)) =
            *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(first + 4));
        *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(first + 4)) = node_arg;
        *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(anchor)) = node;
        return;
    }

    *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(node)) = anchor;
    uint32_t enc = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(anchor + 4));
    *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(node + 4)) = enc;
    uint32_t rel = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(base));
    if (static_cast<int32_t>(enc) < 0) {
        *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(~enc)) = node;
    } else {
        if (static_cast<int32_t>(rel) < 0) {
            uint32_t first = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(anchor));
            rel = anchor - *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(first + 4));
        }
        *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(enc + rel)) = node;
    }
    *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(anchor + 4)) = node_arg;
}
