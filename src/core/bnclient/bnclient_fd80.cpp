// 1.13c BNClient+0xFD80 (ECX = out id, EDX = destination): take the oldest message off the received queue.
// Under the module lock: an empty queue returns 0. Otherwise the head's payload (its length at +0x124, bytes from +4)
// is copied to the destination, its first dword goes to the out id, the head is freed (ECX = the node, EDX = the
// source file name, stack 0x89, 0) and the queue head becomes the node's successor (+0x128). Returns the length.
#include "bnclient_send.h"
#include <cstring>

// 1.13c BNClient+0xFD80
extern "C" uint32_t bnclient_fd80(const BnCallEnv& e, uint32_t* head_slot, uint32_t* out_id, uint8_t* dst) {
    const uint32_t head = *head_slot;
    if (head == 0) return 0;
    const uint8_t* node = (const uint8_t*)(uintptr_t)head;
    uint32_t len, id, next;
    memcpy(&len, node + 0x124, 4);
    memmove(dst, node + 4, len);
    memcpy(&id, node, 4);
    *out_id = id;
    memcpy(&len, node + 0x124, 4);
    memcpy(&next, node + 0x128, 4);
    bn_call(e, 0x80f0, 0, head, BN_MOD(0x1ace4), 0, 0, 0x89, 0);
    *head_slot = next;
    return len;
}
