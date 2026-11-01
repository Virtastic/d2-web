#include "native_list.h"
#include "storm_link.h"
#include <cstdlib>
#include <emscripten.h>

// 1.13c D2Client+0x2400 (166 B): doubly-linked-list splice over the list's link offset (storm_link.h,
// d2c_link::splice2400). ABI (retail): EAX = base, EBX = node object, stack = mode, anchor object.
// Native entry: pointer-width words; the FN probe drives the same rule on retail's 32-bit words.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_2400(uintptr_t base, uintptr_t node_arg, uint32_t mode, uintptr_t anchor_arg) {
    d2c_link::splice2400<uintptr_t>(base, node_arg, mode, anchor_arg);
}

// 1.13c D2Client+0x2F00: native list construction; retail's +4 sentinel becomes one pointer word.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2f00(uintptr_t* self) {
    const uintptr_t head = reinterpret_cast<uintptr_t>(self + 1);
    self[0] = 0; self[1] = head; self[2] = ~head;
}
// 1.13c D2Client+0x2FF0: unlink one link (d2c_link::unlink), native words.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2ff0(uintptr_t* self) {
    d2c_link::unlink<uintptr_t>(reinterpret_cast<uintptr_t>(self));
}
// 1.13c D2Client+0x30A0: EDI = list; detach every linked object (d2c_link::drain30a0); freeing is the caller's.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_30a0(uintptr_t* self) {
    d2c_link::drain30a0<uintptr_t>(reinterpret_cast<uintptr_t>(self));
}
// 1.13c D2Client+0x02EC0 (wraps 0x2580): drain then detach the embedded sentinel pair.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2ec0(void* self) {
    auto* list = static_cast<uintptr_t*>(self);
    d2_client_30a0(list);
    d2_client_2ff0(list + 1);
}
// The same rule on retail's 32-bit words (FN probe, stdcall(list), ret 4; EAX is not defined by the function).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2ec0_w32(uint32_t list) {
    d2c_link::drain30a0<uint32_t>(list);
    d2c_link::unlink<uint32_t>(list + 4);
}
// 1.13c D2Client+0x02F70: ESI = mode; stack = list, extra size, line. Allocates size + three words
// (d2c_link::alloc2f70); a failed allocation still splices (retail passes EBX = 0 to 0x2400).
extern "C" EMSCRIPTEN_KEEPALIVE void* d2_client_2f70(
    uintptr_t list, int32_t size, uint32_t line, uint32_t mode, D2ClientListAlloc alloc) {
    uintptr_t node = d2c_link::alloc2f70<uintptr_t>(list, size, line, mode, [alloc](uint32_t bytes, uint32_t tag) {
        return reinterpret_cast<uintptr_t>(alloc(bytes, ".?AUTMESSAGESOURCE@@", 0xFFFFFFFEu, tag));
    });
    return reinterpret_cast<void*>(node);
}
