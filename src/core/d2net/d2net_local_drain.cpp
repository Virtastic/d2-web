// The client's per-frame pump over the local connection, in the order of 1.13c D2Client+0x43920: every
// system packet first (0xaf sets the compression flag, 0xb0 clears it), then every game packet, one
// take each into a 512-byte buffer, until both lists are empty. The game packets, whole and in the
// order the server queued them, are what the dispatcher receives; this hands them to a reader outside
// the module as one run. The run grows with what is pending: nothing is cut.
#include "d2net_local.h"
#include <emscripten.h>
#include <vector>

static std::vector<uint8_t> g_run;

extern "C" {
EMSCRIPTEN_KEEPALIVE const uint8_t* d2net_local_drain() {
    uint8_t buf[0x204];
    g_run.clear();
    while (d2net_local_take_system(buf, 0x200) != 0xFFFFFFFFu) {
        if (buf[0] == 0xaf) g_d2netLocal.compressed = 1;
        else if (buf[0] == 0xb0) g_d2netLocal.compressed = 0;
    }
    for (uint32_t n; (n = d2net_local_take(buf, 0x204)) != 0xFFFFFFFFu;)
        g_run.insert(g_run.end(), buf, buf + (n < 0x204 ? n : 0x204));
    if (g_run.empty()) g_run.reserve(1);
    return g_run.data();
}
EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_drain_length() { return static_cast<uint32_t>(g_run.size()); }
EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_mode() { return g_d2netLocal.mode; }
EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_queued() { return g_d2netLocal.queued; }
EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_taken() { return g_d2netLocal.taken; }
EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_unframed() { return g_d2netLocal.unframed; }
EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_compressed() { return g_d2netLocal.compressed; }
}
