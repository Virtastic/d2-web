#include "native_list.h"
#include <cstdlib>
#include <emscripten.h>

// 1.13c D2Client+0x2400 (166 B): doubly-linked-list splice. Same two-word node encoding as
// 0x10B0/0x1030 -- [0] successor (0 = unlinked), [1] predecessor, negative = ~target, otherwise
// node + (enc - successor[1]). Differences from 0x10B0: the node and anchor are arguments offset
// into base[0] when non-zero (0 selects the embedded sentinel at base+4), the raw argument word is
// stored in the back-link, and base[0] is reloaded at each of its three uses. The unlink reloads
// the node words after the first store because that store can alias the node. ABI (retail):
// EAX = base, EBX = node argument, stack = mode, anchor argument.
static void unlink_2400(uintptr_t node) {
    uintptr_t next = *reinterpret_cast<const uintptr_t*>(static_cast<uintptr_t>(node));
    if (next == 0) return;
    uintptr_t enc = *reinterpret_cast<const uintptr_t*>(static_cast<uintptr_t>(node + sizeof(uintptr_t)));
    uintptr_t target;
    if (static_cast<intptr_t>(enc) < 0) {
        target = ~enc;
    } else {
        target = node + (enc - *reinterpret_cast<const uintptr_t*>(static_cast<uintptr_t>(next + sizeof(uintptr_t))));
    }
    *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(target)) = next;
    next = *reinterpret_cast<const uintptr_t*>(static_cast<uintptr_t>(node));
    enc = *reinterpret_cast<const uintptr_t*>(static_cast<uintptr_t>(node + sizeof(uintptr_t)));
    *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(next + sizeof(uintptr_t))) = enc;
    *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(node)) = 0;
    *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(node + sizeof(uintptr_t))) = 0;
}

// 1.13c D2Client+0x2400
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_2400(uintptr_t base, uintptr_t node_arg, uint32_t mode, uintptr_t anchor_arg) {
    uintptr_t node = node_arg
        ? *reinterpret_cast<const uintptr_t*>(static_cast<uintptr_t>(base)) + node_arg
        : base + sizeof(uintptr_t);
    unlink_2400(node);

    uintptr_t anchor = anchor_arg
        ? *reinterpret_cast<const uintptr_t*>(static_cast<uintptr_t>(base)) + anchor_arg
        : base + sizeof(uintptr_t);

    if (mode != 1) {
        uintptr_t first = *reinterpret_cast<const uintptr_t*>(static_cast<uintptr_t>(anchor));
        *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(node)) = first;
        *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(node + sizeof(uintptr_t))) =
            *reinterpret_cast<const uintptr_t*>(static_cast<uintptr_t>(first + sizeof(uintptr_t)));
        *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(first + sizeof(uintptr_t))) = node_arg;
        *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(anchor)) = node;
        return;
    }

    *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(node)) = anchor;
    uintptr_t enc = *reinterpret_cast<const uintptr_t*>(static_cast<uintptr_t>(anchor + sizeof(uintptr_t)));
    *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(node + sizeof(uintptr_t))) = enc;
    uintptr_t rel = *reinterpret_cast<const uintptr_t*>(static_cast<uintptr_t>(base));
    if (static_cast<intptr_t>(enc) < 0) {
        *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(~enc)) = node;
    } else {
        if (static_cast<intptr_t>(rel) < 0) {
            uintptr_t first = *reinterpret_cast<const uintptr_t*>(static_cast<uintptr_t>(anchor));
            rel = anchor - *reinterpret_cast<const uintptr_t*>(static_cast<uintptr_t>(first + sizeof(uintptr_t)));
        }
        *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(enc + rel)) = node;
    }
    *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(anchor + sizeof(uintptr_t))) = node_arg;
}

// 1.13c D2Client+0x2F00: native list construction; retail's +4 sentinel becomes one pointer word.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_2f00(uintptr_t* self) {
    const uintptr_t head = reinterpret_cast<uintptr_t>(self + 1);
    self[0] = 0; self[1] = head; self[2] = ~head;
}
// 1.13c D2Client+0x2FF0: unlink a native intrusive pair, preserving retail's reload order.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_2ff0(uintptr_t* self) {
    unlink_2400(reinterpret_cast<uintptr_t>(self));
}
// 1.13c D2Client+0x30A0: detach every owned pair; ownership/freeing belongs to the caller.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_30a0(uintptr_t* self) {
    while (static_cast<intptr_t>(self[2]) > 0)
        d2_fn_d2client_2ff0(reinterpret_cast<uintptr_t*>(self[0] + self[2]));
}
// 1.13c D2Client+0x02EC0 (wraps 0x2580): drain then detach the embedded sentinel pair.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_2ec0(void* self) {
    auto* list = static_cast<uintptr_t*>(self);
    d2_fn_d2client_30a0(list);
    d2_fn_d2client_2ff0(list + 1);
}
// 1.13c D2Client+0x02F70: scalar allocation/debug arguments remain 32-bit; the two links
// and callback slot are native pointer words. The trailing size is retail's extra byte count.
extern "C" EMSCRIPTEN_KEEPALIVE void* d2_client_2f70(
    uintptr_t list, int32_t size, uint32_t line, uint32_t mode, D2ClientListAlloc alloc) {
    const uint32_t bytes = static_cast<uint32_t>(size) + 3u * sizeof(uintptr_t);
    auto* node = static_cast<uintptr_t*>(alloc(bytes, "__AUTMESSAGESOURCE__", 0xFFFFFFFEu, line | 8u));
    if (!node) return nullptr;
    node[0] = 0; node[1] = 0;
    if (mode) d2_client_2400(list, reinterpret_cast<uintptr_t>(node), mode, 0);
    return node;
}
