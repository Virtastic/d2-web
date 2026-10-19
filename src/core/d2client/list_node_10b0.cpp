#include "native_list_abi.h"
#include <emscripten.h>

// Prior art used: has no CRT intrusive-list implementation; D2MOO
// has no D2Client source. Rule and reload ordering checked against cached 1.13c +0x10B0.

// Native node words carry pointers; this is not a serialized retail-memory adapter.
// Retail two-word offsets scale to native words, including the complemented predecessor.
// 1.13c D2Client+0x10B0 helper: unlink a node {native next; native prev-encoded} of the intrusive list in
// 0x1030/0x11F0. prev is either a pointer stored complemented (~v) or a roll-forward value decoded as
// node + (v - next->prev); address arithmetic wraps at native pointer width.
static void unlink_10b0(uintptr_t* node) {
    uintptr_t next = node[0];
    if (!next) return;
    uintptr_t prev = node[1];
    uintptr_t slot;
    if (static_cast<intptr_t>(prev) < 0) slot = ~prev;
    else slot = reinterpret_cast<uintptr_t>(node) + (prev - *reinterpret_cast<uintptr_t*>(next + sizeof(uintptr_t)));
    *reinterpret_cast<uintptr_t*>(slot) = next;
    // Reload after the predecessor store: it can alias either node word.
    *reinterpret_cast<uintptr_t*>(node[0] + sizeof(uintptr_t)) = node[1];
    node[0] = 0;
    node[1] = 0;
}

// 1.13c D2Client+0x10B0: ECX = this, EAX = container base; stack arg1 = node (0 -> EAX+1), arg2 = tag.
// Unlinks arg1-or-base[1] when non-empty, then (arg2 != 1) links it after the selected anchor or
// (arg2 == 1) before it, storing arg1 (raw, 0 included) as the neighbour's prev word.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_10b0(uintptr_t* ecxThis, uintptr_t* eaxBase, uintptr_t* arg1, int32_t arg2) {
    uintptr_t* node = arg1 ? arg1 : eaxBase + 1;
    unlink_10b0(node);
    uintptr_t* self = ecxThis ? ecxThis : eaxBase + 1;
    if (arg2 != 1) {
        uintptr_t n = self[0];
        node[0] = n;
        node[1] = *reinterpret_cast<uintptr_t*>(n + sizeof(uintptr_t));
        *reinterpret_cast<uintptr_t*>(n + sizeof(uintptr_t)) = reinterpret_cast<uintptr_t>(arg1);
        self[0] = reinterpret_cast<uintptr_t>(node);
        return;
    }
    node[0] = reinterpret_cast<uintptr_t>(self);
    node[1] = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(self) + sizeof(uintptr_t));
    uintptr_t prev = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(self) + sizeof(uintptr_t));
    intptr_t off = static_cast<intptr_t>(eaxBase[0]);
    if (static_cast<intptr_t>(prev) < 0) {
        *reinterpret_cast<uintptr_t*>(~prev) = reinterpret_cast<uintptr_t>(node);
        *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(self) + sizeof(uintptr_t)) = reinterpret_cast<uintptr_t>(arg1);
        return;
    }
    if (off < 0) {
        uintptr_t first = *reinterpret_cast<uintptr_t*>(self[0] + sizeof(uintptr_t));
        off = static_cast<intptr_t>(reinterpret_cast<uintptr_t>(self) - first);
    }
    *reinterpret_cast<uintptr_t*>(static_cast<uintptr_t>(prev + static_cast<uintptr_t>(off))) = reinterpret_cast<uintptr_t>(node);
    *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(self) + sizeof(uintptr_t)) = reinterpret_cast<uintptr_t>(arg1);
}
