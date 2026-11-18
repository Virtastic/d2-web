// The client's per-frame pump over the local connection: one call of the game body of 1.13c D2Client+0x43920
// (src/core/d2client/d2client_43920.cpp) with the live callees. The takes are the local connection's
// (D2Net +0x71E0 / +0x7200 over +0x6AC0); the system flag is D2Client+0x11C39C; the dispatch step (+0xACE20)
// appends each game packet, whole and in the order the server queued it, to the run handed to the client's
// dispatcher outside the module (d2_realm_client_player_feed, which runs d2c_ace20 per packet). The run grows
// with what is pending: nothing is cut.
// System 0xB3 / 0xB4 are realm records (a fragment, a realm notice) that the local server never queues; they
// are taken and dropped (+0xACD70's flush +0xAC530 and +0xACBF0's +0x43550 are not on this path).
// A game packet of 0x201..0x204 bytes overruns retail's 512-byte buffer: the bytes past 0x200 are whatever the
// stack held. The run carries zeros there so the stream stays framed.
#include "d2net_local.h"
#include "../d2client/d2client_43920.h"
#include <emscripten.h>
#include <vector>

static std::vector<uint8_t> g_run;

static void runAppend(const uint8_t* packet, int32_t size) {
    if (size == -1) return;   // +0xACE20's first test: the empty take dispatches nothing
    const uint32_t n = static_cast<uint32_t>(size), kept = n < 0x200 ? n : 0x200;
    g_run.insert(g_run.end(), packet, packet + kept);
    g_run.insert(g_run.end(), n - kept, 0);
}
static void systemRecord(const uint8_t*) {}

extern "C" {
EMSCRIPTEN_KEEPALIVE const uint8_t* d2net_local_drain() {
    static const D2c43920Env live = { d2net_local_take_system, d2net_local_take, systemRecord, systemRecord, runAppend, &g_d2c_11c39c };
    g_run.clear();
    d2c_43920(&live);
    if (g_run.empty()) g_run.reserve(1);
    return g_run.data();
}
EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_drain_length() { return static_cast<uint32_t>(g_run.size()); }
EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_mode() { return g_d2netLocal.mode; }
EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_queued() { return g_d2netLocal.queued; }
EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_taken() { return g_d2netLocal.taken; }
EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_unframed() { return g_d2netLocal.unframed; }
EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_compressed() { return static_cast<uint32_t>(g_d2c_11c39c); }
}
