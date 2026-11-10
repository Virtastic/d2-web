// 1.13c D2Net+0x63E0 (ordinal 10002), live form of its local branch: the server's send to a client.
// On a local connection (mode 1 or 2) the run is split and appended to the client's lists
// (D2Net+0x6BD0) and its length returned; the lists have no bound and the send never fails. With no
// local connection there is no socket here, so nothing is sent and 0 is returned.
//
// Retail ends the process when a run is longer than 0x204 bytes or does not split by the size table.
// Ours keeps such a run whole as one packet (cut to 0x204) and counts it in `unframed`, so a sender
// whose packet length disagrees with 1.13c is measured instead of lost.
#include "d2net_local.h"
#include <cstring>
#include <emscripten.h>

static D2NetPktNode* allocNode() { return new D2NetPktNode; }


extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_send(const uint8_t* buf, uint32_t len) {
    D2NetLocalConn& c = g_d2netLocal;
    if ((c.mode != 1 && c.mode != 2) || !buf || len == 0) return 0;
    // the run splits exactly when the size rule walks it to its end
    uint32_t off = 0, packets = 0;
    while (len <= 0x204 && off < len) {
        uint32_t entry = 0;
        const uint32_t sz = d2_realm_d2net_6750(buf + off, len - off, &entry);
        if (sz < 1 || sz > len - off) break;
        off += sz; ++packets;
    }
    if (off == len && d2_realm_d2net_6bd0(buf, static_cast<int>(len), &c.q, allocNode, 0)) {
        c.queued += packets;
        return len;
    }
    D2NetPktNode* n = allocNode();
    const uint32_t sz = len < 0x204 ? len : 0x204;
    memcpy(n->data, buf, sz);
    n->size = sz; n->tick = 0; n->next = nullptr;
    D2NetPktNode** head = buf[0] > 0xae && buf[0] <= 0xb4 ? &c.q.second : &c.q.normal;
    while (*head) head = &(*head)->next;
    *head = n;
    ++c.queued; ++c.unframed;
    return len;
}
