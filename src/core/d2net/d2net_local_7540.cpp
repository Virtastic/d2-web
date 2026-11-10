// 1.13c D2Net+0x7540 (ordinal 10034), live form: open the connection. Both list heads are cleared and
// the mode stored; a local mode (1 or 2) then sends itself the two-byte system record {0xAF, 0x00}
// ("not compressed") through the send, so the first packet a client takes from the system list is that
// one. The receive thread of the socket modes is not part of the local path: any other mode leaves the
// connection closed here.
#include "d2net_local.h"
#include <emscripten.h>

D2NetLocalConn g_d2netLocal = {};

static void freeList(D2NetPktNode** head) {
    while (D2NetPktNode* n = *head) { *head = n->next; delete n; }
}

extern "C" {
EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_open(uint32_t mode) {
    freeList(&g_d2netLocal.q.normal);
    freeList(&g_d2netLocal.q.second);
    g_d2netLocal = {};
    if (mode != 1 && mode != 2) return 0;
    g_d2netLocal.mode = mode;
    const uint8_t rec[2] = { 0xAF, 0x00 };
    d2net_local_send(rec, 2);
    return 1;
}
// The connection's end: no retail routine frees unread packets of a local connection (the process
// exits with them); ours releases them so a page that leaves a game holds nothing.
EMSCRIPTEN_KEEPALIVE void d2net_local_close() {
    freeList(&g_d2netLocal.q.normal);
    freeList(&g_d2netLocal.q.second);
    g_d2netLocal.mode = 0;
}
}
