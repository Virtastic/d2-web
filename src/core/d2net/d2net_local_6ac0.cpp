// 1.13c D2Net+0x6AC0, live form: take the first packet of a list. An empty list gives -1. Otherwise
// min(len, 0x204) bytes of the head packet are copied to dst, the head moves to the next packet, the
// taken one is released and its size returned. (Mode 2 holds a packet back until it is 500 ms old;
// the local game runs mode 1.)
#include "d2net_local.h"

uint32_t d2net_local_6ac0(D2NetPktNode** head, uint8_t* dst, uint32_t len) {
    D2NetPktNode* node = *head;
    if (!node) return 0xFFFFFFFFu;
    const uint32_t n = len < 0x204 ? len : 0x204, size = node->size;
    for (uint32_t i = 0; i < n; ++i) dst[i] = node->data[i];
    *head = node->next;
    delete node;
    ++g_d2netLocal.taken;
    return size;
}
